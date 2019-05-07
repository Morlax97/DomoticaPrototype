const testDevice = document.querySelector('.device');
const deviceState = document.querySelector('.device__state');

const toggleState = () => {
    if (deviceState.classList.contains('device__state--off')) {
        deviceState.classList.remove('device__state--off');
        deviceState.innerHTML = 'on';
        deviceState.classList.add('device__state--on');
    } else if (deviceState.classList.contains('device__state--on')) {
        deviceState.classList.remove('device__state--on');
        deviceState.innerHTML = 'off';
        deviceState.classList.add('device__state--off');
    }
}

testDevice.addEventListener('click', toggleState);