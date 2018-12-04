import requests

url = "https://api.eospark.com/api?module=contract&action=get_contract_trx_info&apikey=a5b3c19bd55ed4d8839d2426a8a1dce5&account=dacincubator&action_name=transfer&page=1&size=20"

response = requests.request("GET", url)

print(response.text)