# Cloud Run scaling experiment

Experiment on how quickly Cloud Run instances can scale up on GCP. We deliberately specify `--concurrency=1`, so there's only one request running on an instance at a time.

## Build and deploy

```bash
GCP_PROJECT=leo-dev-290304
GCP_REGION=australia-southeast1
ARTIFACT_REGISTRY=ar-sydney

gcloud config set project $GCP_PROJECT

DOCKER_IMAGE=$GCP_REGION-docker.pkg.dev/$GCP_PROJECT/$ARTIFACT_REGISTRY/received-time:latest

gcloud builds submit --tag $DOCKER_IMAGE .

gcloud run deploy time-received --region=$GCP_REGION --allow-unauthenticated --platform=managed --concurrency=1 --max-instances=1000 --image=$DOCKER_IMAGE
```

## Running the test

```bash
CLOUD_RUN_URL=$(gcloud run services describe time-received --platform managed --region $GCP_REGION --format 'value(status.url)')

mkdir /tmp/timing
cd /tmp/timing
for i in $(seq 1 1000); do curl -s --retry 100 $CLOUD_RUN_URL/received_time > timing_$i.txt & done

# Wait until all jobs have finished, then run:
grep -h Received timing_*.txt | sort > timing.txt
```

## Results

Ideally, the first and last entry of `timing.txt` wouldn't be far apart timewise.

In practice, `curl` needs to retry a lot due to _"The request was aborted because there was no available instance"_ errors, e.g. leading to this first and last entry:

```text
Received at: Sun Jun 27 07:25:51 2021
...
Received at: Sun Jun 27 07:26:16 2021
```

Every request nominally takes 10s, but due to the upscaling delay, request processing start times get distributed over a 25s window. I.e. overall processing takes 35s, whereas with instant upscaling it would only take 10s.
