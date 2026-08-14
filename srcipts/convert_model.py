#!/usr/bin/env python3
"""
Custom Model Conversion Script
Converts quantized TFLite models into C-source arrays for Ethos-U55 NPU execution.
"""

import os
import sys
import shutil
import argparse
import tempfile
from pathlib import Path

# Compiler toolchain imports
import mera
from mera import Platform, Target

def main():
    parser = argparse.ArgumentParser(description="Convert TFLite to MCU C-source arrays.")
    parser.add_argument("--input", type=str, required=True, help="Input model path (.tflite)")
    parser.add_argument("--output-dir", type=str, default="embedded_c", help="Output directory for generated C code")
    parser.add_argument("--accelerator-config", type=str, default="ethos-u55-256", help="Vela accelerator config")
    parser.add_argument("--system-config", type=str, default="Ethos_U55_High_End_Embedded", help="Vela system config")
    parser.add_argument("--memory-mode", type=str, default="Shared_Sram", help="Vela memory mode")
    parser.add_argument("--optimise", type=str, default="Performance", help="Vela optimisation target")

    args = parser.parse_args()

    model_path = Path(args.input).resolve()
    if not model_path.exists():
        print(f"Error: Input model not found at {model_path}")
        sys.exit(1)

    output_dir = Path(args.output_dir).resolve()
    if output_dir.exists():
        shutil.rmtree(output_dir, ignore_errors=True)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Auto-detect if external memory (OSPI) is needed (threshold: 0.8 MB)
    file_size_mb = model_path.stat().st_size / (1024 * 1024)
    needs_ospi = file_size_mb > 0.8

    print(f"Loading model: {model_path.name} ({file_size_mb:.2f} MB)")
    if needs_ospi:
        print(f"  -> Model exceeds 0.8MB, enabling external OSPI memory mapping.")

    # Initialize the custom compiler framework in a temporary directory to avoid space-in-path bugs
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_out = Path(temp_dir) / "output"
        with mera.Deployer(str(temp_out), overwrite=True) as deployer:
            # 1. Load the quantized TFLite model
            loader = mera.ModelLoader(deployer)
            compiled_model = loader.from_tflite(str(model_path))

            print(f"Deploying for NPU ({args.accelerator_config})...")
            
            # 2. Configure Vela and C-code generation parameters
            vela_config = {
                'enable_ospi': needs_ospi,
                'sys_config': 'RA8P1',  # Target hardware profile
                'memory_mode': args.memory_mode,
                'accel_config': args.accelerator_config,
                'optimise': args.optimise,
                'verbose_all': False,
            }
            
            mcu_config = {
                'suffix': '',
                'weight_location': 'flash',
                'use_x86': False,
            }

            # 3. Generate the C-source output arrays
            deployer.deploy(
                compiled_model,
                mera_platform=Platform.MCU_ETHOS,
                target=Target.MCU,
                vela_config=vela_config,
                mcu_config=mcu_config,
                enable_ref_data=False
            )

        # Move the generated output to the actual destination
        shutil.copytree(temp_out, output_dir, dirs_exist_ok=True)

    print(f"Conversion complete. C-source arrays generated in: {output_dir}")

if __name__ == "__main__":
    main()
