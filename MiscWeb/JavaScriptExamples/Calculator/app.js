// This calculator is described in this web page:
//    https://medium.com/@kshitijsharma94/building-a-simple-calculator-with-html-css-and-javascript-1bda25ce3d80


// This function is immediately executed.  It is a way to define a
// scope of short names that can be used in the scripts.  For instance,
// the name 'buttons' refers to all the html-elements with class='btn'
(function() {
    let screen = document.querySelector('.screen');
    let buttons = document.querySelectorAll('.btn');
    let clear = document.querySelector('.btn-clear');
    let equal = document.querySelector('.btn-equal');

    // Now add event listeners to the 'click' event for the various
    // html elements.

    // The click-event for 'buttons', elements with the class '.btn'.
    // The elements with class '.btn' are the digits whose value is the
    // corresponding digit string, and the numeric  operators +,-,*,/
    // whose value is the corresponding operator string.  Each of these
    // has an event listener which appends the value string to the 'screen'
    // element.
    buttons.forEach(function(button) {
        button.addEventListener('click', function(e) {
            let value = e.target.dataset.num;
            if (value !== undefined) {
                screen.value += value;
            }
        });
    });

    // The click-event for 'equal', the element of class .btn-equal.
    // Simply evaluate the screen.value, and replace its contents
    // by its evaluation.
    equal.addEventListener('click', function(e) {
        if (screen.value === '') {
            screen.value = "Please enter";
        } else {
            try {
                // The JavaScript eval function evaluates an expr-string.
                // The 'let' keyword is to introduced scoped variable 'answer'.
                let answer = eval(screen.value);
                screen.value = answer;
            } catch (error) {
                screen.value = "Error";
            }
        }
    });

    // The click-event for 'clear', the element of class .btn-clear.
    // Simply set the screen.value to the empty string.
    clear.addEventListener('click', function(e) {
        screen.value = "";
    });

// And we are done
//  }  - completes the definition of the scoping function
//  )  - since the scoping function is unnamed, bracket its definition.
//  () - Then immediately evaluate the scoping function, which assigns
//       all the event listeners.
//     - Alternatively, we could name the scoping function (after the
//       keyword 'function'), and then immediately call it in the
//       outermost scope.
})()

    


