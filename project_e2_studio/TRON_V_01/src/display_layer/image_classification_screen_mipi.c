#include "hal_data.h"
#include <stdio.h>
#include <string.h>
#include "camera_control.h"
#include "common_util.h"
#include "display_layer.h"
#include "bg_font_18_full.h"
#include "time_counter.h"
#include "display_layer_config.h"
#include "ai_application_config.h"
#include "modern_font_a8.h"
#define MAX_STR_LEN 15
#define INFERENCE_ROW_HEIGHT    55 // Increased spacing for progress bars
#define TEXT_AREA_WIDTH         320
#define TEXT_AREA_HEIGHT        560
#define PERCENTAGE_OFF          220 // Pushed to the right edge

void do_image_classification_screen(bool ai_result_new);
display_runtime_cfg_t glcd_layer_change_1;
display_runtime_cfg_t glcd_layer_change_2;

static void process_str(const char* input, char* output, int max_len);
static void print_inf_time(void);
static bool overlay_drawn = false;
static char local_str[5][32] = {0};
static char local_prob[5][8] = {0};

const char* coco_labels[] = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
    "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
    "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
    "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
    "hair drier", "toothbrush"
};

// =========================================================================
// MODERN UI PRIMITIVES (CustomTkinter / Fluent Design Style)
// =========================================================================
static void print_modern_font(d2_device *handle, int x, int y, d2_color text_color, char *str)
{
    size_t len = strlen(str);

    // Enable Alpha blending so the text opacity blends with the panel color perfectly
    d2_setblendmode(handle, d2_bm_alpha, d2_bm_one_minus_alpha);

    // Set the solid color of the text (e.g. 0xFFFFFFFF for White, 0xFF00FFCC for Neon Cyan)
    d2_setcolor(handle, 0, text_color);

    int cur_x = x;
    int cur_y = y;

    for(uint32_t pos = 0; pos < len; pos++)
    {
        char c = str[pos];
        const modern_font_char_t *char_img = get_modern_font_char(c);

        if(char_img != NULL)
        {
            // CRITICAL: d2_mode_alpha8 tells DAVE2D that the array only contains opacity data!
            d2_setblitsrc(handle, (void*)char_img->pixel_data, char_img->width, char_img->width, char_img->height, d2_mode_alpha8);

            d2_blitcopy(handle,
                    char_img->width, char_img->height,
                    0, 0,
                    (char_img->width) << 4, (char_img->height) << 4,
                    cur_x << 4, cur_y << 4,
                    d2_tm_filter);

            cur_x += char_img->width;
        }
    }
}
static void draw_rounded_rect(d2_device *handle, int x, int y, int w, int h, int r, d2_color color)
{
    d2_setcolor(handle, 0, color);
    d2_renderbox(handle, (x + r) << 4, y << 4, (w - 2 * r) << 4, h << 4);
    d2_renderbox(handle, x << 4, (y + r) << 4, w << 4, (h - 2 * r) << 4);
    d2_rendercircle(handle, (x + r) << 4,         (y + r) << 4,         r << 4, 0);
    d2_rendercircle(handle, (x + w - r) << 4,     (y + r) << 4,         r << 4, 0);
    d2_rendercircle(handle, (x + r) << 4,         (y + h - r) << 4,     r << 4, 0);
    d2_rendercircle(handle, (x + w - r) << 4,     (y + h - r) << 4,     r << 4, 0);
}

static void draw_ctk_panel(d2_device *handle, int x, int y, int w, int h, int r) {
    // Enable Alpha Blending to draw translucent drop shadows
    d2_setblendmode(handle, d2_bm_alpha, d2_bm_one_minus_alpha);

    // Draw 2 layers of soft drop shadows (expanding outwards)
    d2_setalpha(handle, 40);
    draw_rounded_rect(handle, x - 2, y + 2, w + 4, h + 4, r + 2, 0xFF000000);
    d2_setalpha(handle, 20);
    draw_rounded_rect(handle, x - 4, y + 4, w + 8, h + 8, r + 4, 0xFF000000);

    // Restore opacity
    d2_setalpha(handle, 255);

    // Main Panel Card (Sleek Dark Blue/Gray)
    draw_rounded_rect(handle, x, y, w, h, r, 0xFF2A2D3E);
}

static void draw_ctk_progressbar(d2_device *handle, int x, int y, int w, int h, float percentage, d2_color accent_color)
{
    int radius = h / 2;
    // Background track (Very dark gray)
    draw_rounded_rect(handle, x, y, w, h, radius, 0xFF1C1E29);

    int fill_w = (int)((w * percentage) / 100.0f);
    if (fill_w > radius * 2) {
        draw_rounded_rect(handle, x, y, fill_w, h, radius, accent_color);
    } else if (fill_w > 0) {
        d2_setcolor(handle, 0, accent_color);
        d2_rendercircle(handle, (x + radius)<<4, (y + radius)<<4, radius<<4, 0);
    }
}

// =========================================================================

static void print_inf_time(void)
{
    uint32_t time = (uint32_t)(application_processing_time.ai_inference_time_ms);
    char time_str[8] = {'0', '0', '0', '0', ' ', 'm', 's', '\0'};
    time_str[0] += (char)((time / 1000) % 10);
    time_str[1] += (char)((time / 100) % 10);
    time_str[2] += (char)((time / 10) % 10);
    time_str[3] += (char)(time % 10);

    print_bg_font_18(d2_handle, 80, 155, DISPLAY_FONT_SCALING, (char*)time_str);

    // Draw a visual representation of latency (Max 200ms = 100% full bar)
    float latency_pct = (time / 200.0f) * 100.0f;
    if (latency_pct > 100.0f) latency_pct = 100.0f;

    d2_color latency_color = (latency_pct > 50.0f) ? 0xFFFF3333 : 0xFF00FFCC; // Red if slow, Cyan if fast
    draw_ctk_progressbar(d2_handle, 160, 158, 120, 12, latency_pct, latency_color);
}

static void process_str(const char* input, char* output, int max_len) {
    int i;
    for (i = 0; input[i] != '\0' && i < max_len - 1; i++) {
        if (input[i] == ',') break;
        output[i] = input[i];
    }
    for(; i < max_len - 1; i++){
        output[i] = ' ';
    }
    output[max_len - 1] = '\0';
}

void do_image_classification_screen(bool ai_result_new)
{
    d2_point vpos = 260; // Starting Y for detections
    d2_point hpos = 25;

    d2_framebuffer(d2_handle, (void *)fb_foreground, TEXT_AREA_WIDTH, TEXT_AREA_WIDTH, TEXT_AREA_HEIGHT, d2_mode_rgb565);
    d2_startframe(d2_handle);

    // Enable Anti-Aliasing for perfectly smooth Apple-like corners!
    d2_setantialiasing(d2_handle, 1);

    if (!overlay_drawn)
    {
        // 1. Draw Global Background (Deep OLED Black/Blue)
        d2_setcolor(d2_handle, 0, 0xFF12141C);
        d2_renderbox(d2_handle, 0, 0, TEXT_AREA_WIDTH << 4, TEXT_AREA_HEIGHT << 4);

        // 2. Draw Floating Panels (Drop Shadows + Rounded Corners)
        draw_ctk_panel(d2_handle, 10, 10, 300, 80, 12);  // Header Panel
        draw_ctk_panel(d2_handle, 10, 105, 300, 90, 12); // Performance Panel
        draw_ctk_panel(d2_handle, 10, 210, 340, 340, 12); // Detections Panel

        // 3. Draw Static Text inside Panels
        print_modern_font(d2_handle, 25, 25, DISPLAY_FONT_SCALING, (char*)"EDGESIGHT OS");
        print_modern_font(d2_handle, 25, 55, DISPLAY_FONT_SCALING, (char*)"AI Vision System v3.0");

        print_modern_font(d2_handle, 25, 120, DISPLAY_FONT_SCALING, (char*)"Ethos-U55 NPU Status");
        print_modern_font(d2_handle, 25, 155, DISPLAY_FONT_SCALING, (char*)"Time:");

        print_modern_font(d2_handle, 25, 225, DISPLAY_FONT_SCALING, (char*)"Active Tracking");

        R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_BLEN, BSP_IO_LEVEL_HIGH);
        overlay_drawn = true;
    }
    else if(ai_result_new)
    {
        // Erase the old latency reading using a sleek panel-colored box
        d2_setcolor(d2_handle, 0, 0xFF2A2D3E);
        d2_renderbox(d2_handle, 80 << 4, 150 << 4, 210 << 4, 30 << 4);
        print_inf_time();

        for(uint8_t i = 0; i < AI_MAX_DETECTION_NUM; i++)
        {
            char processed_str[MAX_STR_LEN] = {0};

            // Cleanly erase the entire row for this index so we don't have text overlap
            d2_setcolor(d2_handle, 0, 0xFF2A2D3E); // Match panel color exactly
            d2_renderbox(d2_handle, 20 << 4, (vpos + INFERENCE_ROW_HEIGHT*i) << 4, 280 << 4, 50 << 4);

            if(g_ai_detection[i].m_w > 0) {
                int cls_id = g_ai_detection[i].category;
                if(cls_id < 0 || cls_id >= 80) cls_id = 0;

                process_str(coco_labels[cls_id], processed_str, MAX_STR_LEN);
                sprintf(local_str[i],"%s", processed_str);

                float prob_float = g_ai_detection[i].prob * 100.0f;
                sprintf(local_prob[i],"%02d%%", (int)prob_float);

                // Print Object Name (Left) and Percentage (Right)
                print_bg_font_18(d2_handle, hpos, vpos + INFERENCE_ROW_HEIGHT*i, NORMAL_FONT_SCALING, (char*)local_str[i]);
                print_bg_font_18(d2_handle, PERCENTAGE_OFF, vpos + INFERENCE_ROW_HEIGHT*i, NORMAL_FONT_SCALING, (char*)local_prob[i]);

                // Draw CustomTkinter-style smooth progress bar!
                draw_ctk_progressbar(d2_handle, 25, vpos + INFERENCE_ROW_HEIGHT*i + 25, 270, 10, prob_float, 0xFFFF007F);
            }
        }
    }

    d2_endframe(d2_handle);
    SCB_CleanDCache_by_Addr((uint32_t *)fb_foreground, TEXT_AREA_WIDTH * TEXT_AREA_HEIGHT * 2);

    (void)R_GLCDC_LayerChange(&g_plcd_display.p_ctrl, &glcd_layer_change_2, DISPLAY_FRAME_LAYER_2);
}

