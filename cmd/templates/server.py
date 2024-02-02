import logging
from http.server import BaseHTTPRequestHandler, HTTPServer
from typing import List, TypedDict
import json
import os

LogT = TypedDict('LogT', { 'path' : str, 'data' : dict })
port = os.environ.get('PORT', "8000")

class Log:
    def __init__(self, logs : List[LogT] = []):
        self.logs = logs
    def add_to_log(self, log : LogT):
        # self.logs.append(log)
        logging.info(log)

logs = Log()
logger = logging.getLogger()
logger.setLevel(logging.INFO)

class HTTPRequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = self.headers.get('content-length')
        if content_length is not None:
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()

            length = int(content_length)
            data = self.rfile.read(length).decode('utf8')
            logs.add_to_log({ 'data' : json.loads(data), 'path' : self.path })
            self.wfile.write("{}".encode('utf-8'))
        else:
            self.send_response(403)
        self.end_headers()

    def do_GET(self):
        content_length = self.headers.get('content-length')
        if content_length is not None:
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()

            length = int(content_length)
            self.wfile.write("{}".encode('utf-8'))
            logs.add_to_log({
                'data' : json.loads(self.rfile.read(length).decode('utf-8')), 
                'path' : self.path
            })
        else:
            self.send_response(403)
        self.end_headers()

if __name__ == '__main__':
    server = HTTPServer(('0.0.0.0', int(port)), HTTPRequestHandler)
    logging.info('Server Running on 0.0.0.0:{0}'.format(port))
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    server.server_close()
    logging.info('Stopping httpd...\n')