# DubButton

Attiny + IR led == Dub at the push of a button

```
When we come home from work we need our dub fix.
Our smart TV is extremely slow and unresponsive.
The mobile interface is not any better.
Automation to the rescue!
```

It's basically a one button remote that:

- Powers up the TV
- Opens the youtube app
- Searches a random query based on a list of keywords
- Plays the first returned video

![Breadboard Circuit](circuit/circuit.png)

```
The primary button (PB1) does the full sequence.
The secondary button (PB0) skips the power-up step (for when the TV is already on).
There's a third button that drops reset to ground in case we need to stop it mid-sequence.
The yellow led is just to have a visual feedback when it sends commands.
```
