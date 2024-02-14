let puertaIzquierda = false;
let puertaDerecha = false;
let ledIzquierda = false;
let ledDerecha = false;
let activarIzquierda = false;
let activarDerecha = false;

let isChecked = false;

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
			document.getElementById('led_derecho').checked = true;
		} else if (mensaje == 'derecha_on') {
			document.getElementById('led_derecho').checked=false;
		} else if (mensaje == 'izquierda_off') {
			document.getElementById('led_izquierdo').checked=true;
		} else if (mensaje == 'izquierda_on') {
			document.getElementById('led_izquierdo').checked = false;
		} else if (
			mensaje == 'comportamiento_izquierda_on' ||
			mensaje == 'comportamiento_izquierda_silencioso_on'
		) {
			activarIzquierda = true;
			document.getElementById('activar_izquierda').checked = true;
		} else if (
			mensaje == 'comportamiento_izquierda_off' ||
			mensaje == 'comportamiento_izquierda_silencioso_off'
		) {
			activarIzquierda = false;
			document.getElementById('activar_izquierda').checked = false;
		} else if (mensaje == 'comportamiento_derecha_on' || mensaje == 'comportamiento_derecha_silencioso_on') {
			activarDerecha = true;
			document.getElementById('activar_derecha').checked = true;
		} else if (
			mensaje == 'comportamiento_derecha_off' ||
			mensaje == 'comportamiento_derecha_silencioso_off'
		) {
			activarDerecha = false;
			document.getElementById('activar_derecha').checked = false;
		}
		if (!isChecked) {
			document.getElementById('check').checked = true;
			isChecked = true;
		}
		console.log(mensaje);

		// Agregar el mensaje al área de mensajes
		agregarMensaje(mensaje, 'recibido');
	};
};

conectarWebSocket(); // Iniciar la conexión WebSocket

const controlLED = (elementId) => {
	const checkbox = document.getElementById(elementId);
	let message;

	switch (elementId) {
		case 'puerta_izquierda':
			message = checkbox.checked ? 'puerta_izquierda_on' : 'puerta_izquierda_off';
			break;
		case 'puerta_derecha':
			message = checkbox.checked ? 'puerta_derecha_on' : 'puerta_derecha_off';
			break;
		case 'led_izquierdo':
			message = checkbox.checked ? 'led_izquierdo_on' : 'led_izquierdo_off';
			break;
		case 'led_derecho':
			message = checkbox.checked ? 'led_derecho_on' : 'led_derecho_off';
			break;
		case 'activar_izquierda':
			message = checkbox.checked ? 'activar_led_izquierdo' : 'desactivar_led_izquierdo';
			break;
		case 'activar_derecha':
			message = checkbox.checked ? 'activar_led_derecho' : 'desactivar_led_derecho';
			break;
		case 'check':
			isChecked = false;
			message = checkbox.checked ? 'check' : 'check';
			break;
	}

	if (message) {
		socket.send(message);
		agregarMensaje(message, 'enviado');
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
