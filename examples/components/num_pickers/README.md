# Number Pickers

You can use integer pickers and float pickers.

![Num pikcers](https://github.com/matyalatte/tuw/assets/69258547/2b0d8299-b400-43da-8cf4-c21ca1e11f09)

```json
"components": [
    {
        "type": "int",
        "label": "Integer",
        "inc": 10,
        "wrap": true
    },
    {
        "type": "float",
        "label": "Floating-point number",
        "min": 0,
        "max": 1,
        "inc": 0.01,
        "digits": 2
    }
]
```

If the `type` is `int` or `float`, it will be a number picker.  
There are some optional keys for them.

## `min` and `max`

`min` and `max` are optional keys to set the range of allowable values.  
The default range is `[0, 100]`.  

## `inc`

`inc` is an optional key to set increment value (that will be added when clicking the arrow keys.)  
The default value is 1 for `int` pickers, and 0.1 for `float` pickers.

## `digits`

`digits` is an optional key for `float` pickers.  
It sets the number of digits after the decimal point.  
0 means it will be an `int` picker.  

## `wrap`

`wrap` is an optional key to wrap at the `min` and `max`.  
The minimum value will become the maximum value when clicking the down arrow.  
The maximum value will become the minimum value when clicking the up arrow.  

## Notes

Note that both pickers will use double-precision floating-point numbers for their attributes.  
They won't work properly with numbers that have many digits (like extremely large numbers.)
