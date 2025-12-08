#
#  GoogleFindMyTools - A set of tools to interact with the Google Find My API
#  Copyright © 2024 Leon Böttger. All rights reserved.
#
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
import undetected_chromedriver as uc

def find_chrome():
    return r"C:\Program Files\Google\Chrome\Application\chrome.exe"
def get_options():
    options = webdriver.ChromeOptions()
    options.add_argument("--start-maximized")
    options.add_argument("--disable-extensions")
    options.add_argument("--disable-gpu")
    options.add_argument("--no-sandbox")

    options.add_argument(r"--user-data-dir=C:\Users\Marti\AppData\Local\Google\Chrome\User Data")
    options.add_argument(r"--profile-directory=Default")
    return options



def create_driver():
    """Create a Chrome WebDriver with undetected_chromedriver."""

    driver_path = r"C:\Users\Marti\Downloads\chromedriver-win64\chromedriver-win64\chromedriver.exe"

    chrome_options = get_options()
    chrome_options.binary_location = find_chrome()
    try:

        service = Service(driver_path)
        driver = webdriver.Chrome(service=service, options=chrome_options)
        driver.get("https://www.youtube.com/feed/subscriptions")
        print(f"[ChromeDriver] ChromeDriver started using your mom")
        return driver
    except Exception as e:
        print(f"[ChromeDriver] ChromeDriver failed using path lul: {e}")
        raise Exception("What the helly")


if __name__ == '__main__':
    create_driver()
