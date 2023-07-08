## [README in English](https://github.com/WessTorn/HNS-JumpStats/blob/main/README_ENG.md)

# ВНИМАНИЕ! Статистика находится на стадии бета-теста, есть баги.

# HNS-JumpStats

Hide'n'Seek статистика прыжков для Counter-Strike 1.6

## Требование
- [ReHLDS](https://dev-cs.ru/resources/64/)
- [Amxmodx 1.9.0](https://dev-cs.ru/resources/405/)
- [Reapi (last)](https://dev-cs.ru/resources/73/updates)
- [ReGameDLL (last)](https://dev-cs.ru/resources/67/updates)

## Прыжки
- LongJump
- HighJump
- WeirdJump
- CountJump (Drop / Up)
- Multi CountJump (Drop / Up)
- Stand-Up CountJump (Drop / Up)
- Multi Stand-Up CountJump (Drop / Up)
- BhopJump (Ladder / Drop / Up)
- Stand-Up BhopJump (Ladder / Drop / Up)
- BhopInDuck (Ladder / Drop / Up)
- DuckBhop (Ladder / Drop / Up)
- LadderJump

## Описание

### Основная статистика
![stats1](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/ef21fd88-a348-42d9-99ec-1588e196bdb4) ![stats2](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/b69af4f2-1ef6-4c19-934a-1f963b7750d9) ![stats5](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/3532d455-5ded-4d3c-8233-bc7b1be9b12d)

![stats3](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/97c7b379-874d-4adb-97a2-03164eac9bc6) ![stats4](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/df1b2208-9d1c-4863-add3-941b1c4f5114) ![stats6](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/fb2b9f98-790e-476e-86d1-3cdd3f67ad66)

- В первой строчке статистики указано название прыжка. Также показывает тип прыжка (Ladder / Drop / Up / SideWays / BackWards). И кол-во даков.
- Дистанция прыжка.
- Далее идут все элементы престрейфа (начиная с последнего).
- Конечная скорость - максимальная скорость в конце прыжка.
- Кол-во стрейфов

### Статистика стрейфов

![strafe](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/8591efa7-1a42-4612-abc9-228dc188ed00)

1. Зажатые клавиши.
2. Gain, то есть прирост в скорости, полученный на соответствующем стрейфе.
3. Loss, то есть потеря в скорости на соответствующем стрейфе.
4. Кол-во фреймов, которое длился стрейф.
5. Эффективность стрейфа, то есть отношение количества стрейфов, которые давали прирост скорости, к общему количеству фреймов для каждого стрейфа в процентах.
6. Общее кол-во Gain за все стрейфы.
7. Общая эффективность стрейфов (прыжка).
8. Информация о блоке, jof и land (подробнее смотрите ниже).

### Block Jof Land

![jumpoff_landing](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/d25817ff-0239-4864-904c-9a331d895cd5)

### Престрейфы (prespeed - jof, speed, speed)

![prespeed](https://github.com/WessTorn/HNS-JumpStats/assets/63194135/ee3d850a-7739-480d-b9c1-8ac023ad5666)

В prespeed (hud) отображаются jof, тип пресрейфа, speed, FOG и prestrafe pre - скорость до прыжка или дака (перед попаданием на землю) / post -  скорость после

FOG (frames on the ground) - в двух словах, это кол-во frame (кадров) на земле.
- Зачем нам показывают FOG и что мы должны знать?

  FOG нам показывают во время бхопов и даков (dd), тем самым мы понимаем сколько мы находимся на земле и сколько теряем скорость.

  Проще говоря, чем меньше FOG, тем меньше вы потеряете скорость. Скорость престрейфа (pre/post) нам показывает сколько мы потеряли скорости на земле.

  Но! Из-за особенности движка если во время бхопов у вас скорость будет выше 299.973 u/s, то вы потеряете много скорости в любом случае.

## Команды в чат
| Команда | Описание |
| :-: | :-: |
| ljsmenu / ljs | Главное меню |
| strafe / strafestats | Показыть/скрыть статистику по стрейфам |
| stats / ljstats | Показыть/скрыть основную статистику |
| chatinfo / ci | Показыть/скрыть информацию о прыжках в чате |
| showspeed / speed | Показыть/скрыть скорость (HUD) |
| showpre / pre | Показыть/скрыть престрейфы (HUD) |
| jumpoff / jof | Показыть/скрыть jof (HUD) |

## Квары

| Cvar                 | Default    | Description |
| :------------------- | :--------: | :--------: |
| js_prefix | Jump |  |
| js_stats_red | 0 |  |
| js_stats_green | 200 |  |
| js_stats_blue | 60 |  |
| js_failstats_red | 200 |  |
| js_failstats_green | 10 |  |
| js_failstats_blue | 50 |  |
| js_stats_x | -1.0 |  |
| js_stats_y | 0.7 |  |
| js_strafe_x | 0.7 |  |
| js_strafe_y | 0.35 |  |
| js_prespeed_red | 145 |  |
| js_prespeed_green | 145 |  |
| js_prespeed_blue | 145 |  |
| js_prespeed_x | -1.0 |  |
| js_prespeed_y | 0.55 |  |
| js_hud_stats | 2 |  |
| js_hud_strafe | 3 |  |
| js_hud_pre | 1 |  |

## Спасибки

[borjomi](https://forums.alliedmods.net/showthread.php?t=141586) - Надеюсь живой, использую алгоритмы uq_jumpstats

[ufame](https://github.com/ufame) - Мой ментор, наставил меня на верный путь.

[Kpoluk](https://github.com/Kpoluk) - Просто гений, вдохновлялся его kz-rush статистикой, использую его основные алгоритмы.

[Garey](https://github.com/Garey27) - Это бренд.
