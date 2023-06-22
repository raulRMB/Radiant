FROM ubuntu:18.04

# Unreal refuses to run as root user, so we must create a user to run as
# Docker uses root by default
RUN useradd --system ue
USER ue

EXPOSE 7777/udp

WORKDIR /server

COPY --chown=ue:ue . /server
USER root
CMD chown -R ue.ue $PF_SERVER_LOG_DIRECTORY && su ue -c "./RadiantServer.sh --1v1"