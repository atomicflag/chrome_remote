#!/bin/bash -xue

exec chromium \
--headless \
--disable-gpu \
--disable-translate \
--disable-extensions \
--disable-background-networking \
--safebrowsing-disable-auto-update \
--disable-sync \
--metrics-recording-only \
--disable-default-apps \
--no-first-run \
--mute-audio \
--hide-scrollbars \
--remote-debugging-port=9009
