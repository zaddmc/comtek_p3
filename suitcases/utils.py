import random


def hex_to_rgb(hex_color):
    """Convert hex color code to RGB values easier to math on."""
    hex_color = hex_color.lstrip("#")

    red = int(hex_color[0:2], 16)
    green = int(hex_color[2:4], 16)
    blue = int(hex_color[4:6], 16)
    return red, green, blue


def normalize_rgb_value(color_value):
    """range 0-255 to 0-1 for gamme correction"""
    return color_value / 255.0


def apply_gamma_correction(normalized_color):
    """Apply gamma correction. (stackoverflow conversion something about sRGB)"""
    if normalized_color <= 0.03928:
        return normalized_color / 12.92
    else:
        return ((normalized_color + 0.055) / 1.055) ** 2.4


def calculate_luminance(red, green, blue):
    """Calculate brightness of an RGB color."""
    normalized_red = normalize_rgb_value(red)
    normalized_green = normalize_rgb_value(green)
    normalized_blue = normalize_rgb_value(blue)

    corrected_red = apply_gamma_correction(normalized_red)
    corrected_green = apply_gamma_correction(normalized_green)
    corrected_blue = apply_gamma_correction(normalized_blue)
    # Calculate luminance using standard weights from WCAG
    luminance = (
        0.2126 * corrected_red + 0.7152 * corrected_green + 0.0722 * corrected_blue
    )
    return luminance


def calculate_contrast_ratio(luminance1, luminance2):
    """Calculate contrast ratio between two luminance values."""
    lighter_luminance = max(luminance1, luminance2)
    darker_luminance = min(luminance1, luminance2)

    # WCAG contrast ratio formula
    return (lighter_luminance + 0.05) / (darker_luminance + 0.05)


def generate_random_color(text_is_white=True, min_contrast=4.5):
    MAX_ATTEMPTS = 1000

    # Pre-calculate target luminance
    if text_is_white:
        TEXT_LUMINANCE = calculate_luminance(255, 255, 255)
    else:
        TEXT_LUMINANCE = calculate_luminance(0, 0, 0)

    for _ in range(MAX_ATTEMPTS):
        red = random.randint(0, 255)
        green = random.randint(0, 255)
        blue = random.randint(0, 255)

        background_luminance = calculate_luminance(red, green, blue)
        contrast = calculate_contrast_ratio(background_luminance, TEXT_LUMINANCE)

        if contrast >= min_contrast:
            hex_color = f"#{red:02x}{green:02x}{blue:02x}"
            return hex_color

    return "#1F3B73"  # accent color for when no color is found