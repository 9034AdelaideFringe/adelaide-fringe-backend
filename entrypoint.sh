#!/bin/sh

echo '{ "database": "'"$DATABASE_URL"'", "JWTSecret": "'"$JWT_SECRET"'" }' > /app/config.json

exec "$@"
