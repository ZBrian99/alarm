const audios = {
	on: new Audio('on.mp3'),
	off: new Audio('off.mp3'),
	open: new Audio('open.mp3'),
	close: new Audio('close.mp3'),
};
let puertaIzquierda = false;
let puertaDerecha = false;
let ledIzquierda = false;
let ledDerecha = false;
let activarIzquierda = false;
let activarDerecha = false;

let lastMessage = null;
let messageCount = 0;

Object.values(audios).forEach((audio) => {
	audio.preload = 'auto';
});

function reproducirSonido(audioName) {
	const audio = audios[audioName];
	audio.play();
}

const socket = new WebSocket('ws://192.168.1.82:81/');

socket.onopen = function () {
	console.log('Conexión WebSocket abierta.');
	socket.send('web');
	// agregarMensaje('web', 'enviado');

	setInterval(function () {
		socket.send('ping');
		// agregarMensaje('ping', 'enviado');
	}, 5000);
};

socket.onclose = function () {
	console.log('Conexión WebSocket cerrada.');
};

socket.onmessage = function (event) {
	const mensaje = event.data;

	if (mensaje == 'derecha_off') {
		document.body.style.backgroundColor = 'purple';
	} else if (mensaje == 'derecha_on') {
		document.body.style.backgroundColor = 'gray';
	} else if (mensaje == 'izquierda_off') {
		document.body.style.backgroundColor = 'red';
	} else if (mensaje == 'izquierda_on') {
		document.body.style.backgroundColor = 'green';
	} else if (mensaje == 'comportamiento_izquierda_on') {
		activarIzquierda = true;
	} else if (mensaje == 'comportamiento_izquierda_off') {
		activarIzquierda = false;
	} else if (mensaje == 'comportamiento_derecha_on') {
		activarDerecha = true;
	} else if (mensaje == 'comportamiento_derecha_off') {
		activarDerecha = false;
	}

	console.log(mensaje);
	document.getElementById('recibido').innerText = mensaje;

	// Agregar el mensaje al área de mensajes
	agregarMensaje(mensaje, 'recibido');
};

function controlLED(estado) {
	switch (estado) {
		case 'puerta_izquierda':
			if (puertaIzquierda) {
				socket.send('puerta_izquierda_off');
				puertaIzquierda = false;
			} else if (!puertaIzquierda) {
				socket.send('puerta_izquierda_on');
				puertaIzquierda = true;
			}
			break;
		case 'puerta_derecha':
			if (puertaDerecha) {
				socket.send('puerta_derecha_off');
				puertaDerecha = false;
			} else if (!puertaDerecha) {
				socket.send('puerta_derecha_on');
				puertaDerecha = true;
			}
			break;
		case 'led_izquierdo':
			if (ledIzquierda) {
				socket.send('led_izquierdo_off');
				ledIzquierda = false;
			} else if (!ledIzquierda) {
				socket.send('led_izquierdo_on');
				ledIzquierda = true;
			}
			break;
		case 'led_derecho':
			if (ledDerecha) {
				socket.send('led_derecho_off');
				ledDerecha = false;
			} else if (!ledDerecha) {
				socket.send('led_derecho_on');
				ledDerecha = true;
			}
			break;
		case 'activar_izquierda':
			socket.send('activar_desactivar_led_izquierdo');
			break;
		case 'activar_derecha':
			socket.send('activar_desactivar_led_derecho');
			break;
		case 'check':
			socket.send('check');
			break;
	}
	agregarMensaje(estado, 'enviado');
}

function agregarMensaje(mensaje, tipo) {
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
}

function obtenerHoraActual(now) {
	const horas = String(now.getHours()).padStart(2, '0');
	const minutos = String(now.getMinutes()).padStart(2, '0');
	const segundos = String(now.getSeconds()).padStart(2, '0');
	const milisegundos = String(now.getMilliseconds()).padStart(3, '0').slice(0, 2); // Solo tomamos los primeros dos dígitos de los milisegundos

	return `${horas}:${minutos}:${segundos}.${milisegundos}`;
}

function crearNuevoMensaje(horaActual, mensaje, tipo) {
	const nuevoMensaje = document.createElement('p');
	nuevoMensaje.textContent = `${horaActual} ${tipo === 'enviado' ? '📤 ' : '📥 '}${mensaje}`;
	nuevoMensaje.classList.add(tipo === 'enviado' ? 'enviado' : 'recibido');
	return nuevoMensaje;
}

function actualizarMensaje(ultimoMensaje, horaActual, mensaje, tipo, count) {
	ultimoMensaje.textContent = `${horaActual} ${tipo === 'enviado' ? '📤 ' : '📥 '}${mensaje} x${count}`;
}
