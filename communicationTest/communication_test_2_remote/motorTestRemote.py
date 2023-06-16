import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

# Fetch the service account key JSON file contents
cred = credentials.Certificate('motortest-baf91-firebase-adminsdk-59dj3-cd2a82059d.json')
# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(cred, {
    'databaseURL': "https://motortest-baf91-default-rtdb.asia-southeast1.firebasedatabase.app/"
})

ref = db.reference('/motorTest/int')

while(True):
    print("0: STOP, 1: FWD, 2: BACK, 3: LEFT, 4: RIGHT")
    x=int(input("Whats next?  "))
    ref.set(x)