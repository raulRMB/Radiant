import os

from boto3 import session
from botocore.client import Config
from dotenv import load_dotenv
from pydo import Client

load_dotenv()

ACCESS_KEY = os.getenv('ACCESS_KEY')
SECRET_KEY = os.getenv('SECRET_KEY')
DO_TOKEN = os.getenv('DO_TOKEN')

clientDO = Client(DO_TOKEN)
sesh = session.Session()
botoClient = sesh.client(
    's3',
    region_name='nyc3',
    endpoint_url='https://nyc3.digitaloceanspaces.com',
    aws_access_key_id=ACCESS_KEY,
    aws_secret_access_key=SECRET_KEY)

botoClient.upload_file('../Client/dist/launcher Setup 1.0.0.exe', 'rtb', 'downloads/rtClient.exe', {'ACL': 'public-read', 'ContentType': 'application/octet-stream'})
purge_req = {"files": ['downloads/rtClient.exe']}
clientDO.cdn.purge_cache("12148a1d-4fdf-4925-a0c7-fad213c75b7b", purge_req)