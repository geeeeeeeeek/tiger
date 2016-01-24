### Java Code

``` java
int x = 0;
int y = 1;
int z = 4;
// packed-switch
switch(type){
    case 1:
        z = x;
    case 2:
        z += y;
        break;
    default:
        z = y;
}
// loop
while (z > 0){
    z -= y;
}
// nested loop & array
char[] s = new char[]{'H', 'e', 'l', 'l', 'o'};
char[] t = new char[]{'l', 'o'};
int subIndex = -1;
out:for (int i = 0; i < s.length; i++){
    for (int j = 0; j < t.length; j++){
        if (i + j < s.length){
            if (s[i] != t[j]){
                continue out;
            }
        } else {
            continue out;
        }
    }
    subIndex = i;
    break;
}
if (subIndex > 0 && subIndex < s.length){
    for (; subIndex < s.length; subIndex++){
        s[subIndex] = '*';
    }
}
```



### Dex code

``` c
Start	CodeUnits	Instruction
1048	1	const/4 v5, 0
1050	1	const/4 v6, 1
1052	1	const/4 v7, 4
1054	3	packed-switch v10, +0x43
1060	1	move v7, v6
1062	2	if-lez v7, +0x7
1066	1	sub-int/2addr v7, v6
1068	1	goto -0x3
1070	1	move v7, v5
1072	1	add-int/2addr v7, v6
1074	1	goto -0x6
1076	1	const/4 v8, 5
1078	2	new-array v2, v8, [C
1082	3	fill-array-data v2, +0x3d
1088	1	const/4 v8, 2
1090	2	new-array v4, v8, [C
1094	3	fill-array-data v4, +0x41
1100	1	const/4 v3, -1
1102	1	const/4 v0, 0
1104	1	array-length v8, v2
1106	2	if-ge v0, v8, +0x18
1110	1	const/4 v1, 0
1112	1	array-length v8, v4
1114	2	if-ge v1, v8, +0x13
1118	2	add-int v8, v0, v1
1122	1	array-length v9, v2
1124	2	if-ge v8, v9, +0x8
1128	2	aget-char v8, v2, v0
1132	2	aget-char v9, v4, v1
1136	2	if-eq v8, v9, +0x5
1140	2	add-int/lit8 v0, v0, 1
1144	1	goto -0x14
1146	2	add-int/lit8 v1, v1, 1
1150	1	goto -0x13
1152	1	move v3, v0
1154	2	if-lez v3, +0xf
1158	1	array-length v8, v2
1160	2	if-ge v3, v8, +0xc
1164	1	array-length v8, v2
1166	2	if-ge v3, v8, +0x9
1170	2	const/16 v8, 42
1174	2	aput-char v8, v2, v3
1178	2	add-int/lit8 v3, v3, 1
1182	1	goto -0x9
1184	1	return-void
1186	1	nop
1188	8	packed-switch-payload
1204	9	array-payload
1222	1	nop
1224	6	array-payload
```



### Control Flow Graph

![cfg](https://cloud.githubusercontent.com/assets/7262715/12536749/722331b4-c2e9-11e5-83e9-182cc1e48ec5.png)



### Data Dependency Graph

![dpg](https://cloud.githubusercontent.com/assets/7262715/12536745/60b42cee-c2e9-11e5-8bd3-139c92fddea5.png)

