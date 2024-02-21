FROM python:3.10-slim

# Code here is executed as root
RUN useradd -u 8879 -m calici
RUN mkdir /app
RUN chown -R calici:calici /app
COPY ./requirements.txt /app
RUN pip install -r /app/requirements.txt

# Expose the port your app runs on
EXPOSE 3000
EXPOSE 8765

# Code after this is just preparation for running
WORKDIR /app
COPY ./ /app

ENTRYPOINT ["python3", "/app/main.py"]
