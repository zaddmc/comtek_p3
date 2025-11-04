import random

def hex_to_rgb(hex_color):
    """Convert hex color code to RGB values."""
    # Remove the '#' prefix if present
    hex_color = hex_color.lstrip('#')
    
    # Extract red, green, blue components (2 characters each)
    red = int(hex_color[0:2], 16)
    green = int(hex_color[2:4], 16)
    blue = int(hex_color[4:6], 16)
    
    return red, green, blue

def normalize_rgb_value(color_value):
    """Convert RGB value from 0-255 range to 0-1 range."""
    return color_value / 255.0

def apply_gamma_correction(normalized_color):
    """Apply gamma correction to normalized color value."""
    if normalized_color <= 0.03928:
        return normalized_color / 12.92
    else:
        return ((normalized_color + 0.055) / 1.055) ** 2.4

def calculate_luminance(red, green, blue):
    """Calculate the perceived brightness of an RGB color."""
    # Normalize RGB values to 0-1 range
    normalized_red = normalize_rgb_value(red)
    normalized_green = normalize_rgb_value(green)
    normalized_blue = normalize_rgb_value(blue)
    
    # Apply gamma correction to each color component
    corrected_red = apply_gamma_correction(normalized_red)
    corrected_green = apply_gamma_correction(normalized_green)
    corrected_blue = apply_gamma_correction(normalized_blue)
    
    # Calculate luminance using standard weights
    luminance = (
        0.2126 * corrected_red +
        0.7152 * corrected_green + 
        0.0722 * corrected_blue
    )
    
    return luminance

def calculate_contrast_ratio(luminance1, luminance2):
    """Calculate contrast ratio between two luminance values."""
    lighter_luminance = max(luminance1, luminance2)
    darker_luminance = min(luminance1, luminance2)
    
    # WCAG contrast ratio formula
    return (lighter_luminance + 0.05) / (darker_luminance + 0.05)

def get_text_color_suitability(background_luminance):
    """Determine if black or white text provides better contrast."""
    BLACK_LUMINANCE = calculate_luminance(0, 0, 0)      # Pure black
    WHITE_LUMINANCE = calculate_luminance(255, 255, 255)  # Pure white
    
    contrast_with_black = calculate_contrast_ratio(background_luminance, BLACK_LUMINANCE)
    contrast_with_white = calculate_contrast_ratio(background_luminance, WHITE_LUMINANCE)
    
    # WCAG minimum contrast ratio for normal text
    MINIMUM_CONTRAST = 4.5
    
    black_text_suitable = contrast_with_black >= MINIMUM_CONTRAST
    white_text_suitable = contrast_with_white >= MINIMUM_CONTRAST
    
    return black_text_suitable, white_text_suitable, contrast_with_black, contrast_with_white

def generate_random_color():
    """Generate a random color with good text readability."""
    MAX_ATTEMPTS = 1000  # Prevent infinite loops
    
    for attempt in range(MAX_ATTEMPTS):
        # Generate random color
        red = random.randint(0, 255)
        green = random.randint(0, 255)
        blue = random.randint(0, 255)
        hex_color = f"#{red:02x}{green:02x}{blue:02x}"
        
        # Calculate background luminance
        background_luminance = calculate_luminance(red, green, blue)
        
        # Check text suitability
        black_ok, white_ok, _, _ = get_text_color_suitability(background_luminance)
        
        # Return if either black or white text provides sufficient contrast
        if black_ok or white_ok:
            return hex_color
    
    # Fallback color if no suitable color found
    return "#808080"  # Gray
