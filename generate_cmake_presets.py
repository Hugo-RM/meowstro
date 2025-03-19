import json
import os

# Paths to JSON files
config_path = "config.json"
presets_template_path = "CMakePresets.default.json"
output_path = "CMakePresets.json"

# Ensure config.json exists
if not os.path.exists(config_path):
    print(f"Error: {config_path} not found. Please create it from config.default.json and set the correct paths.")
    exit(1)

# Read config.json
with open(config_path, "r") as file:
    config = json.load(file)

# Ensure CMakePresets.default.json exists
if not os.path.exists(presets_template_path):
    print(f"Error: {presets_template_path} not found.")
    exit(1)

# Read CMakePresets.default.json
with open(presets_template_path, "r") as file:
    presets = json.load(file)

# Modify cacheVariables with paths from config.json
preset = presets["configurePresets"][0]["cacheVariables"]
preset["SDL2_INCLUDE_DIR"] = f"{config['SDL2_DIR']}/include"
preset["SDL2_LIBRARY"] = f"{config['SDL2_DIR']}/lib/x64/SDL2.lib"
preset["SDL2MAIN_LIBRARY"] = f"{config['SDL2_DIR']}/lib/x64/SDL2main.lib"

preset["SDL2_IMAGE_INCLUDE_DIR"] = f"{config['SDL2_IMAGE_DIR']}/include"
preset["SDL2_IMAGE_LIBRARY"] = f"{config['SDL2_IMAGE_DIR']}/lib/x64/SDL2_image.lib"

preset["SDL2_MIXER_INCLUDE_DIR"] = f"{config['SDL2_MIXER_DIR']}/include"
preset["SDL2_MIXER_LIBRARY"] = f"{config['SDL2_MIXER_DIR']}/lib/x64/SDL2_mixer.lib"

preset["SDL2_TTF_INCLUDE_DIR"] = f"{config['SDL2_TTF_DIR']}/include"
preset["SDL2_TTF_LIBRARY"] = f"{config['SDL2_TTF_DIR']}/lib/x64/SDL2_ttf.lib"

# Write updated CMakePresets.json
with open(output_path, "w") as file:
    json.dump(presets, file, indent=4)

print(f"Generated {output_path} successfully.")
