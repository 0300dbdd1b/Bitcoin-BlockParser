import json

with open('salut', 'r') as file:
    data = json.load(file)
print(data)
