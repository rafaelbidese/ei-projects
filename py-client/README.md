# Edge Impulse Ingestion API

Dont forget to update your `API_KEY`, `HMAC_KEY` and update values accordingly. Make sure you install all the dependencies:

`pip install -r requirements.txt`

and you can run the example

`python ei-client.py`

This is a working example of a python script that posts an image to the Ingestion API.

Before I saw that the docs have been updated I did a little reverse engineering on the API methods by running the mobile-client on a desktop and inspecting the requests with the Chrome developer tools.

As now is on the documentation, the metadata and the image are sent using a `multipart/form-data`. Luckyly, the python library `requests` can do that for us and save the day.

**Important to save the Pikachu and upload him first.**

[Documentation](https://docs.edgeimpulse.com/reference#ingestion-api)