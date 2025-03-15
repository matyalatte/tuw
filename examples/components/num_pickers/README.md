# Number Pickers

You can use integer pickers and float pickers.

![Num pikcers](https://github.com/matyalatte/tuw/assets/69258547/2b0d8299-b400-43da-8cf4-c21ca1e11f09)

```json
"components": [
    {
        "type": "int",
        "id": "i",
        "label": "Integer",
        "inc": 10,
        "wrap": true
    },
    {
        "type": "float",
        "id": "f",
        "label": "Floating-point number",
        "min": 0,
        "max": 1,
        "inc": 0.01,
        "digits": 2
    }
]
```

If the `type` is `int` or `float`, it will be a number picker.
There are some options for them.

## `min` and `max`

`min` and `max` are options to set the range of allowable values.
The default range is `[0, 100]`.  

## `inc`

`inc` is an option to set increment value (that will be added when clicking the arrow keys.)
The default value is 1 for `int` pickers, and 0.1 for `float` pickers.

## `digits`

`digits` is an option for `float` pickers.
It sets the number of digits after the decimal point.
0 means it is an `int` picker.  

## `wrap`

`wrap` is an option to wrap at the `min` and `max`.
The minimum value becomes the maximum value when clicking the down arrow.
The maximum value becomes the minimum value when clicking the up arrow.  

## Notes

Note that both pickers use double-precision floating-point numbers for their attributes.
They might not work as you expected with numbers which have many digits.  
