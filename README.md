# PSI3422 

Laboratório de Sistemas Eletrônicos

Segunda parte da materia, com o Professor Hae

## Instalacao da VM

Para rodar a VM:

https://www.virtualbox.org/wiki/Downloads

Download da VM:

https://drive.google.com/file/d/1-j_YpJb9kzZI_aikGXyG0pj0rwvNHKk7/view?usp=drive_link

## LINUX

Login do Linux: psi

Senha do Linux: lnx$psi20

Compilacao no Linux: compila programa -ocv -v3

## RASPBERRY

Login do Raspberry PI: pi

Senha do Raspberry PI: rasp23

Compilacao no Rasp: compila programa -ocv

## Criar Pasta Compartilhada Entre VM e RASPBERRY

Montar: sshfs pi@192.168.0.54: /home/psi3422/pi

Desmontar: fusermount -u /home/psi3422/pi 

## Camera

Ligar Camera: ./Captura

## Servidor

Abrir Server: ./server

Abrir Cliente: ./cliente 192.168.160.54
