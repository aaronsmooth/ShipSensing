#!flask/bin/python

from flask import Flask
from flask.ext.restful import Api, Resource

app = Flask(__name__)
api = Api(app)
index = -1
increment = False
mmsiList = [265491000,248264000,311695000,235068882,248265000,636011023,566086000,266211000,257613000,355488000,257565000,249904000,636015819,311000102,248225000,236111887,357481000,248223000]

class MmsiAPI(Resource):
    def get(self):
        global increment, index

        increment = not increment;
        index = (index + 1) % len(mmsiList) if increment else index

        return mmsiList[index]
        

api.add_resource(MmsiAPI, '/mmsi', endpoint = 'mmsi')

if __name__ == '__main__':
    app.run(debug = True)
