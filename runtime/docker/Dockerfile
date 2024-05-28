# Copyright 2023, IVEX NV
# All rights reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential

FROM python:3.11.9-slim-bookworm AS builder

RUN apt update && apt install -y g++ && pip install --upgrade pip && mkdir -p /app

WORKDIR /app

COPY requirements.txt /app/
RUN pip install -r /app/requirements.txt

COPY doc/ /app/doc/
COPY src/ /app/src/

RUN python -m compileall . -f -b && find . -name '*.py' -delete

# Runtime stage
FROM python:3.11.9-slim-bookworm  as runtime

RUN  mkdir -p /app
WORKDIR /app

# Explicitly copy Python site-packages from builder
COPY --from=builder /usr/local/lib/python3.11/site-packages /usr/local/lib/python3.11/site-packages
COPY --from=builder /app/doc /app/doc
COPY --from=builder /app/src/runtime/runtime.pyc /app/src/runtime/

ENV PYTHONUNBUFFERED=1

ENTRYPOINT ["python", "/app/src/runtime/runtime.pyc"]
