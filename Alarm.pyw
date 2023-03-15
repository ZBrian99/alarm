import serial
import pygame
import keyboard

puerto_serial = serial.Serial("COM3", 9600, timeout=0.1)

pygame.init()


def reproducir_audio(filename):
    pygame.mixer.init()
    pygame.mixer.music.load(filename)
    pygame.mixer.music.play()
    pygame.mixer.music.set_volume(0.5)
    while pygame.mixer.music.get_busy():
        pygame.time.Clock().tick(10)
    pygame.mixer.quit()


def enviar_datos():
    puerto_serial.write(b'8')

# pyinstaller --onefile "C:/Users/SheyKer/Downloads/Documents/Alarm/Alarm.pyw"

keyboard.add_hotkey('pausa', enviar_datos)

while True:
    if puerto_serial.in_waiting > 0:
        datos = puerto_serial.read().decode().strip()
        # print(datos)
        if datos == "z":
            reproducir_audio("C:/Users/SheyKer/Music/tuturu.mp3")
        if datos == "0":
            reproducir_audio("C:/Users/SheyKer/Music/close.mp3")
        if datos == "1":
            reproducir_audio("C:/Users/SheyKer/Music/open.mp3")
        if datos == "2":
            reproducir_audio("C:/Users/SheyKer/Music/close.mp3")
        if datos == "3":
            reproducir_audio("C:/Users/SheyKer/Music/open.mp3")
        if datos == "4":
            reproducir_audio("C:/Users/SheyKer/Music/off.mp3")
        if datos == "5":
            reproducir_audio("C:/Users/SheyKer/Music/on.mp3")
    pygame.time.wait(10)
