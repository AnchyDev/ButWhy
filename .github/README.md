# ButWhy
Want to find out why GMs are executing commands?
This module requests that the GM type out a reason for every executed command.

Each of these requests are logged to the `module.butwhy` log so you can read them later.

![ButWhy](https://i.imgflip.com/1rn1j7.jpg?a477072)

## Setup
- Clone this module into `./azerothcore-wotlk/modules`
- Re-run cmake
- Recompile core
- Enable module in config
- Add the appender `Appender.ButWhy=2,4,15,ButWhy.log,w` to your `worldserver.conf`
- Add the logger `Logger.module.butwhy=4,ButWhy` to `your worldserver.conf`

## Contact
I have a [Discord Server](https://discord.gg/xdVPGcpJ8C) you can join for support.
