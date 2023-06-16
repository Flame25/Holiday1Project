import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

# Fetch the service account key JSON file contents
cred = credentials.Certificate('motortest-baf91-firebase-adminsdk-59dj3-7e2743eea3.json')
# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(cred, {
    'databaseURL': "https://motortest-baf91-default-rtdb.asia-southeast1.firebasedatabase.app/"
})

ref = db.reference('/motorTest/int')

while(True):
    x=int(input("whats next?  "))
    ref.set(x)
    print(ref.get())