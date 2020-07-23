import json
import time, hmac, hashlib
import requests


class ImageSample:
    def __init__(self, input_filename, upload_filename):
        self.input_filename = input_filename
        self.upload_filename = upload_filename
        with open(self.input_filename, 'rb') as file: 
            self.size =  len(file.read())
    
    def getFilename(self):
        return self.input_filename

    def getUploadFilename(self):
        return self.upload_filename

    def getSize(self):
        return self.size

    def read(self):
        return open(self.input_filename, 'rb')

class EdgeImpulseClient:
    def __init__(self, API_KEY, HMAC_KEY, device_name, device_type, interval_ms):
        self.HMAC_KEY = HMAC_KEY
        self.API_KEY = API_KEY
        self.device_name = device_name
        self.device_type = device_type
        self.interval_ms = interval_ms
        self.sensors = []
        self.values = []

    def addSensor(self, sensor):
        self.sensors.append(sensor)

    def addValue(self, value):
        self.values.append(value)
    

    def makeEncodedData(self):
        # empty signature (all zeros). HS256 gives 32 byte signature, and we encode in hex, so we need 64 characters here
        emptySignature = ''.join(['0'] * 64)

        data = {
            "protected": {
                "ver": "v1",
                "alg": "HS256",
                "iat": time.time() # epoch time, seconds since 1970
            },
            "signature": emptySignature,
            "payload": {
                "device_name": self.device_name,
                "device_type": self.device_type,
                "interval_ms": self.interval_ms,
                "sensors": self.sensors,
                "values": self.values
            }
        }

        # encode in JSON
        encoded = json.dumps(data)

        # sign message
        signature = hmac.new(bytes(self.HMAC_KEY, 'utf-8'), msg = encoded.encode('utf-8'), digestmod = hashlib.sha256).hexdigest()

        # set the signature again in the message, and encode again
        data['signature'] = signature
        self.encoded = json.dumps(data)

    def makeImagePayload(self, image):
        self.image = image
        self.payload = {
            'message' : ('message', self.encoded, 'application/json'),
            'image' : (image.getFilename(), image.read(), 'image/jpeg')
        }

    def upload(self):
        # and upload the file
        res = requests.post(url='https://ingestion.edgeimpulse.com/api/training/data',
                            files = self.payload,
                            headers={
                                'x-file-name': self.image.getUploadFilename(),
                                'x-api-key': self.API_KEY,
                            })
                            
        if (res.status_code == 200):
            print('Uploaded file to Edge Impulse', res.status_code, res.content)
        else:
            print('Failed to upload file to Edge Impulse', res.status_code, res.content)


def main():
    print("Starting....")
    HMAC_KEY = "YOUR_HMAC_KEY"
    API_KEY = "YOUR_API_KEY"
    device_name = "ac:87:a3:0a:2d:ff"   # you probably should change that
    device_type = "camera"              # you probably should change that
    input_filename = 'pikachu.jpg'
    upload_filename = 'test'            # you probably should change that

    ei_client = EdgeImpulseClient(API_KEY, HMAC_KEY, device_name, device_type, 0)
    image = ImageSample(input_filename, upload_filename)

    ei_client.addSensor({"name":"image","units":"rgba"})
    ei_client.addValue("Ref-BINARY-image/jpeg ("+str(image.size)+" bytes) xyz" )
    ei_client.makeEncodedData()
    ei_client.makeImagePayload(image)

    print("Uploading file...")
    ei_client.upload()


if __name__ == "__main__":
    main()