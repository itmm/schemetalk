(write hello)
(write-line world)
(write-line
 	gauss of 5:
	[divide
		value: [multiply
			value: 5
			with: (add value: 5 to: 1)
		]
		by: 2
	]
)