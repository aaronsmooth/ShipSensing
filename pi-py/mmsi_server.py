#!flask/bin/python

from flask import Flask
from flask.ext.restful import Api, Resource

app = Flask(__name__)
api = Api(app)
index = -1
increment = False
mmsiList = [1,2,3,4,5,6]

class MmsiAPI(Resource):
    def get(self):
        global increment, index

        increment = not increment;
        index = (index + 1) % len(mmsiList) if increment else index

        return {
            "mmsi": mmsiList[index]
        }

api.add_resource(MmsiAPI, '/mmsi', endpoint = 'mmsi')

if __name__ == '__main__':
    app.run(debug = True)
