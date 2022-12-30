function password_length(event) {

    const bar = document.getElementById('password-length');

    var length = event.target.value.length;
    bar.className = ""

    if (length < 5) {
        bar.classList.add('bad');
    }
    else if (length >= 5 && length < 10) {
        bar.classList.add('good');
    }
    else if (length >= 10) {
        bar.classList.add('perfect');
    }
}