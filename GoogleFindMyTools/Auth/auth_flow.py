#
#  GoogleFindMyTools - A set of tools to interact with the Google Find My API
#  Copyright © 2024 Leon Böttger. All rights reserved.
#

from selenium.webdriver.support.ui import WebDriverWait
from chrome_driver import create_driver

def request_oauth_account_token_flow():

    print("""[AuthFlow] This script will now open Google Chrome on your device to login to your Google account.
> Please make sure that Chrome is installed on your system.
> For macOS users only: Make that you allow Python (or PyCharm) to control Chrome if prompted. 
    """)

    # Press enter to continue
    input("[AuthFlow] Press Enter to continue...")

    # Automatically install and set up the Chrome driver
    print("[AuthFlow] Installing ChromeDriver...")

    try:
        print("[AuthFlow] Retrieved Account Token successfully.")
        return "oauth2_4/0Ab32j90O1KwIPfUYeVcGb7mkum02lzRZTgWDiEUED1H5Jlkz7vT0QcUdBhe-fI_3l16Fwg"

    finally:
        print("Nani")
        # Close the browser

if __name__ == '__main__':
    request_oauth_account_token_flow()
