import csv


with open("external/static/misc/branche_bog.csv",newline="",encoding="cp1252") as csv_file:
    reader = csv.DictReader(csv_file,delimiter=";", quotechar='"')
    for row in reader:
        if row["KODE"].isalpha():
            print(row["KODE"],row["NACE_TITEL_ENG"])

