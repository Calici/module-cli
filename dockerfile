ARG PYTHON_VERSION
FROM python:$PYTHON_VERSION-slim as build
WORKDIR /app
COPY . .
RUN pip install . --extra-index-url=https://pypi.cal-tb01.ddnsfree.com
RUN python3 setup.py bdist_wheel

FROM scratch 
COPY --from=build /app/dist /