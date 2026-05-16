#!/usr/bin/python3
import os
import json
import uuid
import html
import sys
import urllib.parse

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
WWW_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, '..'))
IMAGE_DIR = os.path.join(WWW_ROOT, "uploads", "gallery_images")
DATA_FILE = os.path.join(WWW_ROOT, "uploads", "gallery_data.json")
IMAGE_WEB_PATH = "/uploads/gallery_images/"

def ensure_directories():
    os.makedirs(IMAGE_DIR, exist_ok=True)
    os.makedirs(os.path.dirname(DATA_FILE), exist_ok=True)

def parse_multipart_form_data():
    content_type = os.environ.get('CONTENT_TYPE', '')
    if 'multipart/form-data' not in content_type:
        return {}

    boundary = None
    if 'boundary=' in content_type:
        boundary = '--' + content_type.split('boundary=')[1]

    if not boundary:
        return {}

    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length == 0:
        return {}

    body = sys.stdin.buffer.read(content_length)

    form_data = {}
    parts = body.split(boundary.encode())

    for part in parts[1:-1]:
        if not part.strip():
            continue

        if b'\r\n\r\n' in part:
            headers_section, content = part.split(b'\r\n\r\n', 1)
        elif b'\n\n' in part:
            headers_section, content = part.split(b'\n\n', 1)
        else:
            continue

        headers = headers_section.decode('utf-8', errors='ignore')

        name_match = None
        filename_match = None
        for line in headers.split('\n'):
            if 'Content-Disposition:' in line and 'name=' in line:
                if 'name="' in line:
                    start = line.find('name="') + 6
                    end = line.find('"', start)
                    if end > start:
                        name_match = line[start:end]

                if 'filename="' in line:
                    start = line.find('filename="') + 10
                    end = line.find('"', start)
                    if end > start:
                        filename_match = line[start:end]

        if name_match:
            if content.endswith(b'\r\n'):
                content = content[:-2]
            elif content.endswith(b'\n'):
                content = content[:-1]

            if filename_match:
                form_data[name_match] = {'filename': filename_match, 'content': content}
            else:
                form_data[name_match] = content.decode('utf-8', errors='ignore')

    return form_data

def load_pets():
    try:
        with open(DATA_FILE, 'r') as f:
            return json.load(f)
    except (IOError, json.JSONDecodeError):
        return []

def save_pets(pets):
    with open(DATA_FILE, 'w') as f:
        json.dump(pets, f, indent=4)

def handle_delete(params):
    image_id = params.get("id", [None])[0]
    if not image_id:
        return
    pets = load_pets()
    new_pets = [p for p in pets if p['image'] != image_id]
    if len(new_pets) < len(pets):
        try:
            file_path = os.path.join(IMAGE_DIR, image_id)
            if os.path.exists(file_path):
                os.remove(file_path)
        except OSError:
            pass
        save_pets(new_pets)

def handle_upload():
    form_data = parse_multipart_form_data()
    pet_name = form_data.get("pet_name", "Unnamed Pet")
    picture_data = form_data.get("pet_picture")
    if picture_data and isinstance(picture_data, dict) and picture_data.get('filename'):
        filename = picture_data['filename']
        content = picture_data['content']
        extension = os.path.splitext(filename)[1]
        unique_filename = f"{uuid.uuid4()}{extension}"
        file_path = os.path.join(IMAGE_DIR, unique_filename)
        with open(file_path, 'wb') as f:
            f.write(content)
        pets = load_pets()
        pets.append({"name": pet_name, "image": unique_filename})
        save_pets(pets)

def render_gallery():
    ensure_directories()

    query_string = os.environ.get("QUERY_STRING", "")
    params = urllib.parse.parse_qs(query_string)

    if params.get("action", [None])[0] == 'delete':
        handle_delete(params)

    if os.environ.get("REQUEST_METHOD") == "POST":
        handle_upload()

    pets = load_pets()

    print("Content-Type: text/html")
    print()

    if not pets:
        print('<p class="no-pets">No pets uploaded yet. Be the first!</p>')
        return

    for pet in reversed(pets):
        safe_name = html.escape(pet['name'])
        image_url = IMAGE_WEB_PATH + pet['image']
        image_id = html.escape(pet['image'])
        print(f'<div class="team-member">')
        print(f'    <img src="{image_url}" alt="{safe_name}">')
        print(f'    <p><strong>{safe_name}</strong></p>')
        print(f'    <button class="delete-btn" data-id="{image_id}">Delete</button>')
        print(f'</div>')

if __name__ == "__main__":
    try:
        render_gallery()
    except Exception as e:
        print("Content-Type: text/html")
        print()
        print(f'<p class="error">{html.escape(str(e))}</p>')
