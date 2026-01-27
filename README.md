# Figure-It-Out

This project was made in collabaration with MaterialStoryLabs at AAU INNOVATE

# Technical Notes

The project relies on Bluetooth Low Energy (BLE) and is inteded to used through a browser, note that it only works on chromium based browsers like Google Chrome, Microsoft Edge and alike. Excluding browsers like Firefox and Sefari

## Website

This project utilizes a front end website made in Python Django

## Backend

It relies on a ESP32-S3 written with ESP-IDF

# Known Isues

## Screen update

The ESP screen does not always update when locked, this is due to a feature disappering during a refactor

- Presumeably it just needs a "screen_update" function call after BLE lock

## Rolling Codes

The rolling code is unevenly implemted at best
Example is that the rolling code counter can be reset allowing for easy replay attack
