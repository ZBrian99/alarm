#Sistema de Sensores con esp32

Este proyecto consiste en un sistema de sensores conectados a un esp32. Cada sensor activa un LED respectivamente y se comunica a través de un servidor web socket hosteado en la palca con un programa hecho con .net c# para activar una notificación de sonido segun comando recibido por socket en base a los sensores activados. Los sensores pueden ser desactivados mediante un comando enviado desde web o mobile, proximamente mediante atajos del teclado en pc, todo por socket.