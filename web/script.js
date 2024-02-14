let puertaIzquierda = false;
let puertaDerecha = false;
let ledIzquierda = false;
let ledDerecha = false;
let activarIzquierda = false;
let activarDerecha = false;

let lastMessage = null;
let messageCount = 0;

let pingInterval;
let socket;

// const audios = {
// 	on: new Audio('on.mp3'),
// 	off: new Audio('off.mp3'),
// 	open: new Audio('open.mp3'),
// 	close: new Audio('close.mp3'),
// };

// Object.values(audios).forEach((audio) => {
// 	audio.preload = 'auto';
// });

//  reproducirSonido=(audioName)=> {
// 	const audio = audios[audioName];
// 	audio.play();
// }

const conectarWebSocket = () => {
	console.log('Conectar WebSocket...');
	// Cerrar la conexión WebSocket existente si está abierta
	if (socket && socket.readyState === WebSocket.OPEN) {
		console.log('Cerrando conexión WebSocket existente...');
		socket.close();
	}

	// Crear una nueva conexión WebSocket
	socket = new WebSocket('ws://192.168.1.82:81/');

	socket.onopen = () => {
		console.log('Conexión WebSocket abierta.');
		socket.send('web');
		agregarMensaje('Conexión WebSocket abierta.', 'consola');

		if (pingInterval) {
			clearInterval(pingInterval);
		}
		// Crear el intervalo
		pingInterval = setInterval(() => {
			socket.send('ping');
			// agregarMensaje('ping', 'enviado');
		}, 5000);
	};

	socket.onclose = () => {
		console.log('Conexión WebSocket cerrada. Intentando reconectar...');
		agregarMensaje('Conexión WebSocket cerrada. Intentando reconectar...', 'consola');
		clearInterval(pingInterval); // Limpiar el intervalo al cerrar la conexión
		pingInterval = null; // Restablecer el intervalo
		setTimeout(conectarWebSocket, 3000); // Intentar reconectar después de 3 segundos
	};

	socket.onerror = (error) => {
		console.error('Error en la conexión WebSocket:', error);
		console.log('Intentando reconectar...');
		agregarMensaje('Error en la conexión WebSocket - Intentando reconectar...', 'consola');
		clearInterval(pingInterval); // Limpiar el intervalo en caso de error
		pingInterval = null; // Restablecer el intervalo
		setTimeout(conectarWebSocket, 3000); // Intentar reconectar después de 3 segundos
	};

	socket.onmessage = (event) => {
		const mensaje = event.data;

		if (mensaje == 'derecha_off') {
			document.body.style.backgroundColor = 'purple';
			document.getElementById('led_derecho').classList.remove('button-3d--blue-pressed');
			document.getElementById('led_derecho').innerText = 'Led Derecho OFF'; // Cambia el texto del botón a "Apagado"
		} else if (mensaje == 'derecha_on') {
			document.body.style.backgroundColor = 'gray';
			document.getElementById('led_derecho').classList.add('button-3d--blue-pressed');
			document.getElementById('led_derecho').innerText = 'Led Derecho ON'; // Cambia el texto del botón a "Encendido"
		} else if (mensaje == 'izquierda_off') {
			document.body.style.backgroundColor = 'red';
			document.getElementById('led_izquierdo').classList.remove('button-3d--blue-pressed');
			document.getElementById('led_izquierdo').innerText = 'Led Izquierdo OFF'; // Cambia el texto del botón a "Apagado"
		} else if (mensaje == 'izquierda_on') {
			document.body.style.backgroundColor = 'green';
			document.getElementById('led_izquierdo').classList.add('button-3d--blue-pressed');
			document.getElementById('led_izquierdo').innerText = 'Led Izquierdo ON'; // Cambia el texto del botón a "Encendido"
		} else if (
			mensaje == 'comportamiento_izquierda_on' ||
			mensaje == 'comportamiento_izquierda_silencioso_on'
		) {
			activarIzquierda = true;
			document.getElementById('comportamiento-izquierda').classList.add('button-3d--red-pressed');
			document.getElementById('comportamiento-izquierda').innerText = 'Izquierda OFF'; // Cambia el texto del botón a "Izquierda OFF"
		} else if (
			mensaje == 'comportamiento_izquierda_off' ||
			mensaje == 'comportamiento_izquierda_silencioso_off'
		) {
			activarIzquierda = false;
			document.getElementById('comportamiento-izquierda').classList.remove('button-3d--red-pressed');
			document.getElementById('comportamiento-izquierda').innerText = 'Izquierda ON'; // Cambia el texto del botón a "Izquierda ON"
		} else if (mensaje == 'comportamiento_derecha_on' || mensaje == 'comportamiento_derecha_silencioso_on') {
			activarDerecha = true;
			document.getElementById('comportamiento-derecha').classList.add('button-3d--red-pressed');
			document.getElementById('comportamiento-derecha').innerText = 'Derecha OFF'; // Cambia el texto del botón a "Derecha OFF"
		} else if (
			mensaje == 'comportamiento_derecha_off' ||
			mensaje == 'comportamiento_derecha_silencioso_off'
		) {
			activarDerecha = false;
			document.getElementById('comportamiento-derecha').classList.remove('button-3d--red-pressed');
			document.getElementById('comportamiento-derecha').innerText = 'Derecha ON'; // Cambia el texto del botón a "Derecha ON"
		}

		console.log(mensaje);
		document.getElementById('recibido').innerText = mensaje;

		// Agregar el mensaje al área de mensajes
		agregarMensaje(mensaje, 'recibido');
	};
};

conectarWebSocket(); // Iniciar la conexión WebSocket

const controlLED = (estado) => {
	switch (estado) {
		case 'puerta_izquierda':
			if (puertaIzquierda) {
				socket.send('puerta_izquierda_off');
				puertaIzquierda = false;
				agregarMensaje('puerta_izquierda_off', 'enviado');
			} else if (!puertaIzquierda) {
				socket.send('puerta_izquierda_on');
				puertaIzquierda = true;
				agregarMensaje('puerta_izquierda_on', 'enviado');
			}
			break;
		case 'puerta_derecha':
			if (puertaDerecha) {
				socket.send('puerta_derecha_off');
				puertaDerecha = false;
				agregarMensaje('puerta_derecha_off', 'enviado');
			} else if (!puertaDerecha) {
				socket.send('puerta_derecha_on');
				puertaDerecha = true;
				agregarMensaje('puerta_derecha_on', 'enviado');
			}
			break;
		case 'led_izquierdo':
			if (ledIzquierda) {
				socket.send('led_izquierdo_off');
				ledIzquierda = false;
				agregarMensaje('led_izquierdo_off', 'enviado');
			} else if (!ledIzquierda) {
				socket.send('led_izquierdo_on');
				ledIzquierda = true;
				agregarMensaje('led_izquierdo_on', 'enviado');
			}
			break;
		case 'led_derecho':
			if (ledDerecha) {
				socket.send('led_derecho_off');
				ledDerecha = false;
				agregarMensaje('led_derecho_off', 'enviado');
			} else if (!ledDerecha) {
				socket.send('led_derecho_on');
				ledDerecha = true;
				agregarMensaje('led_derecho_on', 'enviado');
			}
			break;
		case 'activar_izquierda':
			socket.send('activar_desactivar_led_izquierdo');
			agregarMensaje('activar_desactivar_led_izquierdo', 'enviado');
			break;
		case 'activar_derecha':
			socket.send('activar_desactivar_led_derecho');
			agregarMensaje('activar_desactivar_led_derecho', 'enviado');
			break;
		case 'check':
			socket.send('check');
			agregarMensaje('check', 'enviado');
			break;
	}
};

const agregarMensaje = (mensaje, tipo) => {
	const mensajesContainer = document.getElementById('mensajes');
	const now = new Date();
	const horaActual = obtenerHoraActual(now);

	if (mensaje === lastMessage) {
		messageCount++;
		const ultimoMensaje = mensajesContainer.firstChild;
		actualizarMensaje(ultimoMensaje, horaActual, mensaje, tipo, messageCount);
	} else {
		if (messageCount > 1) {
			lastMessage = `${lastMessage} x${messageCount}`;
		}
		const nuevoMensaje = crearNuevoMensaje(horaActual, mensaje, tipo);
		mensajesContainer.insertBefore(nuevoMensaje, mensajesContainer.firstChild);
		lastMessage = mensaje;
		messageCount = 1;
	}
};

const obtenerHoraActual = (now) => {
	const horas = String(now.getHours()).padStart(2, '0');
	const minutos = String(now.getMinutes()).padStart(2, '0');
	const segundos = String(now.getSeconds()).padStart(2, '0');
	const milisegundos = String(now.getMilliseconds()).padStart(3, '0').slice(0, 2); // Solo tomamos los primeros dos dígitos de los milisegundos

	return `${horas}:${minutos}:${segundos}.${milisegundos}`;
};

const crearNuevoMensaje = (horaActual, mensaje, tipo) => {
	const nuevoMensaje = document.createElement('p');
	nuevoMensaje.textContent = `${horaActual} ${
		tipo === 'enviado' ? '📤 ' : tipo === 'recibido' ? '📥 ' : '🔴 '
	}${mensaje}`;
	nuevoMensaje.classList.add(tipo === 'enviado' ? 'enviado' : tipo === 'recibido' ? 'recibido' : 'consola');
	return nuevoMensaje;
};

const actualizarMensaje = (ultimoMensaje, horaActual, mensaje, tipo, count) => {
	ultimoMensaje.textContent = `${horaActual} ${
		tipo === 'enviado' ? '📤 ' : tipo === 'recibido' ? '📥 ' : '🔴 '
	}${mensaje} x${count}`;
};
