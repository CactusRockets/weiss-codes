[33mcommit 4d23d569444c07bb1ed288bf3c5f24fb311bd63e[m[33m ([m[1;36mHEAD[m[33m -> [m[1;32mmain[m[33m, [m[1;31morigin/main[m[33m, [m[1;31morigin/HEAD[m[33m)[m
Author: Alec-MM <standardsilvercells@gmail.com>
Date:   Sun Aug 30 21:53:34 2026 -0300

    MPU6050 funcionando com I2C aplicado diretamente sem a biblioteca FASTIMU.h e sd_message do message.h salvando a altura de maneira correta e consistente

[33mcommit 6bbf9fc1c2f68b30288e314340c0c37712391c21[m
Author: Daniel-Alencar <danielalencar746@gmail.com>
Date:   Wed Aug 26 21:22:23 2026 -0300

    feat: adjuts in MDNS

[33mcommit eddf2c41dc5500a1f1dfb174e279777df5ce0262[m
Author: Alec-MM <standardsilvercells@gmail.com>
Date:   Sun Aug 23 00:45:08 2026 -0300

    Ajustes dos pinos da placa de telemetria no codigo

[33mcommit 4b08bbebace129a34de3a02b0ef9d39366c4764a[m
Author: Alec-MM <standardsilvercells@gmail.com>
Date:   Sat Aug 22 21:39:14 2026 -0300

    Mantem apito do buzzer após ativar o paraquedas até a avionica desligar

[33mcommit 38c5dacc25e62e04a9d291543b8a52380c6aa236[m
Author: Alec-MM <standardsilvercells@gmail.com>
Date:   Sat Aug 22 21:30:36 2026 -0300

    Reduz periodo do loop para 200ms e adiciona guarda de sanidade no BMP

[33mcommit 77e1c45f4a4b9f7154a4e0a50161e2fb47555d42[m
Author: Alec-MM <standardsilvercells@gmail.com>
Date:   Fri Aug 21 22:55:19 2026 -0300

    Melhoria no startup do buzzer, correção do endereço do bmp para 0x76

[33mcommit 7c84115cb361eab3b2ccf04a52adf65f489f63ba[m
Author: Andrei Rodrigues <andrei.eng.tech@gmail.com>
Date:   Thu Aug 20 17:18:43 2026 -0300

    update(Avionica): Atualizado estado lógico da atiavação da telemetria.

[33mcommit d0e5812b4bf98f9922b2e0291de9993911a7ea8c[m
Author: Miller <guimigc@protonmail.com>
Date:   Wed Aug 19 23:02:56 2026 -0300

    fix: retornando o código correto para buzzer.h

[33mcommit a6bc02be89906d7568f2863731bcd3405c4aefe4[m
Author: Joison Júnior <joisonjr30720@outlook.com>
Date:   Mon Aug 17 18:11:21 2026 -0300

    Mudança do eixo para leitura da aceleração vertical e alteração do nome da biblioteca local 'server.h'

[33mcommit a2e5288e05081769f22fc0d1a3279e0ed5fa9c90[m
Author: Miller <guimigc@protonmail.com>
Date:   Sat Aug 8 13:39:48 2026 -0300

    Revert "Arquivis buzzer.h e bmp.h atualizados"
    
    This reverts commit c9667c1efb2cc357be50dfddbc8b86e8e17871ad.
    
    Voltando para versão anterior

[33mcommit 1950979495d5ce719b4042cde955000dcce45a8d[m
Author: Miller <guimigc@protonmail.com>
Date:   Fri Aug 7 13:11:13 2026 -0300

    Avionica:bmp.h: Mudando o endereço do BMP388 de 0x76 para 0x77

[33mcommit c9667c1efb2cc357be50dfddbc8b86e8e17871ad[m[33m ([m[1;31morigin/minha-versao[m[33m)[m
Author: Alec-MM <standardsilvercells@gmail.com>
Date:   Thu Aug 6 23:03:50 2026 -0300

    Arquivis buzzer.h e bmp.h atualizados

[33mcommit 9c4867ff274bcdcc024e45ede6fa70dfacb8a6fc[m
Author: Joison Júnior <joisonjr30720@outlook.com>
Date:   Thu Jul 23 10:23:25 2026 -0300

    Adição do KalmanFunc.h ao README

[33mcommit ac5e84e7bab3194be1c14e04bb2840c677871931[m
Author: Joison Júnior <joisonjr30720@outlook.com>
Date:   Wed Jul 22 23:29:28 2026 -0300

    Adição de KalmanFunc.h e execução da função kalman_filter() dentro de getSensorsMeasures(), que está em setup.h

[33mcommit fad0b51165ccd106d6337f468081cc0351f70a78[m
Author: Guilherme Emetério <54158328+DankAlighieri@users.noreply.github.com>
Date:   Wed Jul 22 08:42:21 2026 -0300

    Update README.md

[33mcommit c5ca18f9264d29197cece56d4ca26095dc5fa8ab[m
Merge: 8bd17e2 fe28c5b
Author: Guilherme Emetério <54158328+DankAlighieri@users.noreply.github.com>
Date:   Wed Jul 22 08:41:19 2026 -0300

    Merge pull request #1 from CactusRockets/placaIluminacaoImplementacao
    
    Placa iluminacao implementacao

[33mcommit fe28c5ba351c58cd1b545a1373927de7a9f5ce00[m
Author: Guilherme <guilhermeemeterioif@gmail.com>
Date:   Wed Jul 22 08:19:09 2026 -0300

    feat: README adicionado e atualizado para representar o código da aviônica do foguete Weiss

[33mcommit b30c897a577b9ef4325fd66c541b880988f4eb60[m
Author: Guilherme <guilhermeemeterioif@gmail.com>
Date:   Thu Jul 16 21:20:20 2026 -0300

    Criação do arquivo para a placa de iluminação

[33mcommit 8bd17e2813de869703a8af5b23eb84fc59142903[m
Author: Miller <guimigc@protonmail.com>
Date:   Tue Jul 14 20:47:41 2026 -0300

    code: Avionica: parachute.h: Remoção de parâmetros e funções relacionadas ao skib2 utilizado no código do Joliot. Sem mudanças funcionais na lógica de acionamento do skib
