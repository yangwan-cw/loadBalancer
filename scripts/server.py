#!/usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler
import os
import socket
import json
from datetime import datetime

SERVER_NAME = os.environ.get('SERVER_NAME', 'Unknown')
SERVER_PORT = int(os.environ.get('SERVER_PORT', '80'))

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        response = {
            'server': SERVER_NAME,
            'hostname': socket.gethostname(),
            'ip': socket.gethostbyname(socket.gethostname()),
            'path': self.path,
            'time': datetime.now().isoformat(),
            'message': f'Hello from {SERVER_NAME}!'
        }

        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.send_header('X-Server', SERVER_NAME)
        self.end_headers()
        self.wfile.write(json.dumps(response, indent=2).encode())

    def log_message(self, format, *args):
        print(f"[{SERVER_NAME}] {args[0]}")

if __name__ == '__main__':
    server = HTTPServer(('0.0.0.0', SERVER_PORT), Handler)
    print(f'{SERVER_NAME} running on port {SERVER_PORT}')
    server.serve_forever()