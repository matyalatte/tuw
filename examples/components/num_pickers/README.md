# Number Pickers

You can use int pickers and float pickers.

![Num pikcers](https://user-images.githubusercontent.com/69258547/236002847-4c5237db-de77-4065-9d8e-b128979a91b0.png)

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

`min` and `max` are optional keys to set range of allowable values.  
The default range is `[0, 100]`.  

## `inc`

`inc` is an optional key to set increment value (that will be added when clicking arrow keys.)  
The default value is 1 for `int` pickers, and 0.1 for `float` pickers.

## `digits`

`digits` is an optional key for `float` pickers.  
It can set the number of digits after the decimal point.  
0 means it will be `int` picker.  

## `wrap`

`wrap` is an optional key to wrap at the `min` and `max`.  
The minimum value will be the maximum value when clicking the down arrow.  
The maximum value will be the minimum value when clicking the up arrow.  

## Notes

Note that both of pickers will use double-precision floating-point numbers for their attributes.  
It won't work properly with the numbers have many digits (like extremely large numbers.)
