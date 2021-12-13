%%Vyhotovil: Martin Chlebovec (martinius96@gmail.com)
%%Matlab: R2016b

%%Vizualizácia detekčnej charakteristiky pre ultrazvukový senzor JSN-SR04T (vodotesný)
%%Detekčná charakteristika: 60°
%%Max. hĺbka studne 450 cm

close all;
clear all;
half_angle = 30; %%60° detection angle / 2
Radians = deg2rad( half_angle );
Radians2 = -1*(Radians);
hlbka1 = 0:0.01:0.5;
hlbka2 = 0.5:0.01:1;
hlbka3 = 1:0.01:1.5;
hlbka4 = 1.5:0.01:2;
hlbka5 = 2:0.01:2.5;
hlbka6 = 2.5:0.01:3;
hlbka7 = 3:0.01:3.5;
hlbka8 = 3.5:0.01:4;
hlbka9 = 4:0.01:4.5;
priemer1 = hlbka1*tan(Radians)
priemer2 = hlbka1*tan(Radians2)
priemer3 = hlbka2*tan(Radians)
priemer4 = hlbka2*tan(Radians2)
priemer5 = hlbka3*tan(Radians)
priemer6 = hlbka3*tan(Radians2)

priemer7 = hlbka4*tan(Radians)
priemer8 = hlbka4*tan(Radians2)

priemer9 = hlbka5*tan(Radians)
priemer10 = hlbka5*tan(Radians2)

priemer11 = hlbka6*tan(Radians)
priemer12 = hlbka6*tan(Radians2)

priemer13 = hlbka7*tan(Radians)
priemer14 = hlbka7*tan(Radians2)

priemer15 = hlbka8*tan(Radians)
priemer16 = hlbka8*tan(Radians2)

priemer17 = hlbka9*tan(Radians)
priemer18 = hlbka9*tan(Radians2)
plot (hlbka1, priemer1, '-b')
hold on 
plot (hlbka1, priemer2, '-b')
hold on 
plot (hlbka2, priemer3, '-r')
hold on 
plot (hlbka2, priemer4, '-r')
hold on 
plot (hlbka3, priemer5, '-g')
hold on 
plot (hlbka3, priemer6, '-g')
hold on 
plot (hlbka4, priemer7, '-c')
hold on 
plot (hlbka4, priemer8, '-c')
hold on 
plot (hlbka5, priemer9, '-m')
hold on 
plot (hlbka5, priemer10, '-m')
hold on 
plot (hlbka6, priemer11, '-k')
hold on 
plot (hlbka6, priemer12, '-k')
hold on 
plot (hlbka7, priemer13, '-b')
hold on 
plot (hlbka7, priemer14, '-b')
hold on 
plot (hlbka8, priemer15, '-r')
hold on 
plot (hlbka8, priemer16, '-r')
hold on 
plot (hlbka9, priemer17, '-g')
hold on 
plot (hlbka9, priemer18, '-g')
patch([hlbka1 fliplr(hlbka1)], [priemer1 fliplr(priemer2)], 'b')
patch([hlbka2 fliplr(hlbka2)], [priemer3 fliplr(priemer4)], 'r')
patch([hlbka3 fliplr(hlbka3)], [priemer5 fliplr(priemer6)], 'g')
patch([hlbka4 fliplr(hlbka4)], [priemer7 fliplr(priemer8)], 'c')
patch([hlbka5 fliplr(hlbka5)], [priemer9 fliplr(priemer10)], 'm')
patch([hlbka6 fliplr(hlbka6)], [priemer11 fliplr(priemer12)], 'k')
patch([hlbka7 fliplr(hlbka7)], [priemer13 fliplr(priemer14)], 'b')
patch([hlbka8 fliplr(hlbka8)], [priemer15 fliplr(priemer16)], 'r')
patch([hlbka9 fliplr(hlbka9)], [priemer17 fliplr(priemer18)], 'g')
hold on
plot ([0,4.5], [0,0], '-k', 'LineWidth',3)
title('Detekčná charakteristika senzora JSN-SR04T')
grid minor
xlabel('Hĺbka studne (m)') 
ylabel('Priemer detekovanej plochy (m)') 
