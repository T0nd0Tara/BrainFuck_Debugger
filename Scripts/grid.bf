++++++++++++[->++++>++++<<] puts '0' in c:1 & c:2
>>>++++[-<++++++++++>] adds to c:2 to make it 'X'
++++++++++ puts '\n' in c:3

>+++++ set width to c:4
[->+>+<<] mov c:4 to c:5 & c:6

>>>++++ set height to c:7
[->+>+<<] mov c:7 to c:8 & c:9

> c:8
set c:frames
[
    -
    << c:6
    [
        -
        >>>>>+
        <<<<<
    ]
    reset width
    <c:5
    [->+>+<<] mov c:5 to c:6 & c:7
    >>[-<<+>>] mov c:7 to c:5

    > c:8
]
reset height
> c:9
[-<+<+>>] mov c:9 to c:8 & c:7
<<[->>+<<] mov c:7 to c:9


>>>>c:11
MAIN LOOP
[ frames
    -
    >>>>>+>+ add 1 to c:16 & c:17
    <<<<<< c:11
    <<< c:8
    [ height
        -

        << c:6
        [ width
            -
            >>>>>>>+ add 1 to c:13
            [->+>+<<] mov c:13 to c:14 & c:15
            >>[-<<+>>] mov c:15 to c:13
            >[-<<->>] remove c:16 from c:14

            >[-<+<+>>] mov c:17 to c:16 & c:15
            <<[->>+<<] mov c:15 to c:17
            
            +
            <c:14
            [ if c:14 != c:16
                <<<<<<< c:7
                <<<<<< c:1
                . print c:1
                >>>>>> c:7
                >>>>>>> c:14
                [-] clear c:14
                >[-] clear c:15
                <c:14
            ]>c:15
            [ c:14 == c:16
                [-]
                <<<<<<<< c:7
                <<<<< c:2
                .
                >>>>> c:7
                >>>>>>>> c:15
            ]
            [-]<[-]

            <<<< c:10
            <<<< c:6
        ]
        reset width
        <[->+>+<<] mov c:5 to c:6 & c:7
        >>[-<<+>>] mov c:7 to c:5

        <<<<. print '\n'

        >>>>> c:8
    ]
    reset height
    >[-<+<+>>] mov c:9 to c:8 & c:7
    <<[->>+<<] mov c:7 to c:9
    <<<<.>>>> print '\n'
    
    >>>> c:11
    >>[-]>[-]clear c:13 & c:14
    <<< c:11    
]

0 1   2   3    4 5      6     7 8      9       10 11     12 13     14    15 16    17    
0 '0' 'X' '\n' 0 cWidth width 0 height cHeight 0  frames 0  cIndex index 0  frame cFrame