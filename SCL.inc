; ============================================================
; scl.inc - Sound Creator Library v21.0.0
; Base corrigida (v20) + melhorias: multi-canal de verdade,
; arpejo com semitons musicais reais, pan por instrumento,
; volume por nota, gate (staccato), mudanca de tempo em tempo
; de execucao, efeitos configuraveis por instrumento, e um
; contador simples de vozes ativas para depuracao.
; Testada com rgbasm/rgblink reais (rgbds v0.9.2) e executada
; em emulador (PyBoy) para validar os registradores de som.
; ============================================================

if !def(SCL_INC)
def SCL_INC equ 1

; ============================================================
; VERIFICACAO DE VERSAO
; ============================================================
if !def(__RGBDS_MAJOR__) || !def(__RGBDS_MINOR__)
    fail "SCL requer RGBDS 0.5.0 ou superior"
endc

if __RGBDS_MAJOR__ == 0 && __RGBDS_MINOR__ < 5
    fail "SCL requer RGBDS 0.5.0 ou superior"
endc

; ============================================================
; CONSTANTES DE HARDWARE
; ============================================================

if !def(rNR10)
    def rNR10 equ $FF10
endc
if !def(rNR11)
    def rNR11 equ $FF11
endc
if !def(rNR12)
    def rNR12 equ $FF12
endc
if !def(rNR13)
    def rNR13 equ $FF13
endc
if !def(rNR14)
    def rNR14 equ $FF14
endc

if !def(rNR21)
    def rNR21 equ $FF16
endc
if !def(rNR22)
    def rNR22 equ $FF17
endc
if !def(rNR23)
    def rNR23 equ $FF18
endc
if !def(rNR24)
    def rNR24 equ $FF19
endc

if !def(rNR30)
    def rNR30 equ $FF1A
endc
if !def(rNR31)
    def rNR31 equ $FF1B
endc
if !def(rNR32)
    def rNR32 equ $FF1C
endc
if !def(rNR33)
    def rNR33 equ $FF1D
endc
if !def(rNR34)
    def rNR34 equ $FF1E
endc

if !def(rNR41)
    def rNR41 equ $FF20
endc
if !def(rNR42)
    def rNR42 equ $FF21
endc
if !def(rNR43)
    def rNR43 equ $FF22
endc
if !def(rNR44)
    def rNR44 equ $FF23
endc

if !def(rNR50)
    def rNR50 equ $FF24
endc
if !def(rNR51)
    def rNR51 equ $FF25
endc
if !def(rNR52)
    def rNR52 equ $FF26
endc

if !def(rWavePattern)
    def rWavePattern equ $FF30
endc
if !def(rWavePatternEnd)
    def rWavePatternEnd equ $FF3F
endc

if !def(rLY)
    def rLY equ $FF44
endc
if !def(rIE)
    def rIE equ $FFFF
endc

if !def(B_NR14_RESTART)
    def B_NR14_RESTART equ 7
endc
if !def(B_NR14_LENGTH)
    def B_NR14_LENGTH equ 6
endc
if !def(B_NR52_ENABLE)
    def B_NR52_ENABLE equ 7
endc
if !def(B_NR30_ON)
    def B_NR30_ON equ 7
endc

; ============================================================
; CONSTANTES DE NOTAS
; ============================================================
; Cada constante e o valor de periodo de 11 bits (0-$7FF) gravado em
; NRx3/NRx4. Frequencia mais aguda = periodo menor. O hardware nao
; consegue representar periodo negativo, entao existe um teto de
; agudo (D4s, onde o periodo chega a 0).

if !def(C0)
    def C0  equ $7F8
    def C0s equ $7D0
    def D0  equ $7A8
    def D0s equ $780
    def E0  equ $758
    def F0  equ $730
    def F0s equ $708
    def G0  equ $6E0
    def G0s equ $6B8
    def A0  equ $690
    def A0s equ $668
    def B0  equ $640
endc

if !def(C1)
    def C1  equ $618
    def C1s equ $5F0
    def D1  equ $5C8
    def D1s equ $5A0
    def E1  equ $578
    def F1  equ $550
    def F1s equ $528
    def G1  equ $500
    def G1s equ $4D8
    def A1  equ $4B0
    def A1s equ $488
    def B1  equ $460
endc

if !def(C2)
    def C2  equ $438
    def C2s equ $410
    def D2  equ $3E8
    def D2s equ $3C0
    def E2  equ $398
    def F2  equ $370
    def F2s equ $348
    def G2  equ $320
    def G2s equ $2F8
    def A2  equ $2D0
    def A2s equ $2A8
    def B2  equ $280
endc

if !def(C3)
    def C3  equ $258
    def C3s equ $230
    def D3  equ $208
    def D3s equ $1E0
    def E3  equ $1B8
    def F3  equ $190
    def F3s equ $168
    def G3  equ $140
    def G3s equ $118
    def A3  equ $0F0
    def A3s equ $0C8
    def B3  equ $0A0
endc

; A oitava 4 so tem 4 notas mesmo (nao falta nada): D4s = $000 e o teto
; de agudo do hardware (periodo minimo possivel). Nao existe "oitava 5"
; nesse esquema - o Game Boy nao consegue tocar mais agudo que D4s por
; esse mecanismo.
if !def(C4)
    def C4  equ $078
    def C4s equ $050
    def D4  equ $028
    def D4s equ $000
endc

; ============================================================
; CONSTANTES PARA NOISE
; ============================================================

if !def(NOISE_PERIOD_0)
    def NOISE_PERIOD_0   equ 0
    def NOISE_PERIOD_1   equ $10
    def NOISE_PERIOD_2   equ $20
    def NOISE_PERIOD_3   equ $30
    def NOISE_PERIOD_4   equ $40
    def NOISE_PERIOD_5   equ $50
    def NOISE_PERIOD_6   equ $60
    def NOISE_PERIOD_7   equ $70
    def NOISE_PERIOD_8   equ $80
    def NOISE_PERIOD_9   equ $90
    def NOISE_PERIOD_10  equ $A0
    def NOISE_PERIOD_11  equ $B0
    def NOISE_PERIOD_12  equ $C0
    def NOISE_PERIOD_13  equ $D0
    def NOISE_PERIOD_14  equ $E0
    def NOISE_PERIOD_15  equ $F0
endc

if !def(NOISE_STEP_7BIT)
    def NOISE_STEP_7BIT  equ 0
    def NOISE_STEP_15BIT equ $08
endc

; ============================================================
; CONSTANTES PARA EFEITOS
; ============================================================
; Efeitos 0-15: modulacao continua, aplicada a cada tick enquanto o
; instrumento estiver tocando (arpejo, vibrato, etc. - ver DEF_INSTRUMENT).
; Efeitos >=32: "comandos de linha", processados uma unica vez quando
; o evento e lido (ex.: mudar o andamento da musica).

if !def(SCL_EFFECT_NONE)
    def SCL_EFFECT_NONE         equ 0
    def SCL_EFFECT_ARPEGGIO     equ 1
    def SCL_EFFECT_SLIDE        equ 2
    def SCL_EFFECT_VIBRATO      equ 3
    def SCL_EFFECT_TREMOLO      equ 4
    def SCL_EFFECT_PORTA        equ 5
    def SCL_EFFECT_ECHO         equ 6
    def SCL_EFFECT_CHORUS       equ 7
    def SCL_EFFECT_REVERB       equ 8
    def SCL_EFFECT_FLANGER      equ 9
    def SCL_EFFECT_PITCH_BEND   equ 10
    def SCL_EFFECT_VOLUME_SWELL equ 11
    def SCL_EFFECT_PAN_SWEEP    equ 12
    def SCL_EFFECT_RANDOM       equ 13
    def SCL_EFFECT_GLIDE        equ 14
    def SCL_EFFECT_FILTER       equ 15
endc

; Comando de linha (evento): muda o andamento da musica. O parametro do
; evento passa a ser o novo numero de ticks por linha.
if !def(SCL_EFFECT_SET_TEMPO)
    def SCL_EFFECT_SET_TEMPO equ 32
endc

; A nota ocupa 2 bytes (word - periodo de 11 bits), entao 0 nao pode
; significar "silencio" (D4s tambem vale 0). Por isso existem dois
; valores-sentinela fora da faixa valida (0-$7FF).
if !def(SCL_NOTE_REST)
    def SCL_NOTE_REST equ $FFFE
    def SCL_NOTE_END  equ $FFFF
endc

; Valores de pan por instrumento/canal (usados em DEF_INSTRUMENT e em
; SCL_ApplyChannelPan). O Game Boy suporta pan independente por canal
; de verdade via NR51 (2 bits por canal: esquerda e direita).
if !def(SCL_PAN_LEFT)
    def SCL_PAN_LEFT   equ 0
    def SCL_PAN_RIGHT  equ 1
    def SCL_PAN_MUTE   equ 2
    def SCL_PAN_CENTER equ 3
endc

; ============================================================
; OFFSETS PARA ESTRUTURAS
; ============================================================

def SCL_Instrument_duty        equ 0
def SCL_Instrument_env         equ 1
def SCL_Instrument_sweep       equ 2
def SCL_Instrument_length      equ 3
def SCL_Instrument_waveform    equ 4
def SCL_Instrument_noise       equ 6
def SCL_Instrument_effect      equ 7
def SCL_Instrument_effect_param equ 8
def SCL_Instrument_volume      equ 9
def SCL_Instrument_pan         equ 10
def SCL_Instrument_priority    equ 11
def SCL_Instrument_SIZE        equ 12

def SCL_Song_tempo      equ 0
def SCL_Song_patterns   equ 1  ; word: ponteiro para a lista de padroes
def SCL_Song_bpm        equ 3  ; informativo apenas (nao usado pelo motor)
def SCL_Song_SIZE       equ 4

; SCL_Pattern_tracks agora aponta para uma LISTA de faixas (uma por
; canal usado no padrao), nao mais direto para os dados de uma unica
; faixa - ver ADD_TRACK/END_PATTERN.
def SCL_Pattern_length  equ 0
def SCL_Pattern_tracks  equ 1  ; word: ponteiro para a lista de faixas
def SCL_Pattern_SIZE    equ 3

def SCL_Event_note          equ 0  ; word
def SCL_Event_duration      equ 2
def SCL_Event_instrument    equ 3  ; word
def SCL_Event_effect        equ 5
def SCL_Event_effect_param  equ 6
def SCL_Event_volume        equ 7
def SCL_Event_pan           equ 8
def SCL_Event_gate          equ 9
def SCL_Event_velocity      equ 10
def SCL_Event_SIZE          equ 11

; ============================================================
; VARIAVEIS EM RAM
; ============================================================
; Os arranjos "por canal" reservam 2 bytes por canal (8 bytes no total
; para os 4 canais), mesmo quando o campo logicamente usa so 1 byte -
; o codigo calcula o endereco de cada canal multiplicando (canal-1)
; por 2, entao todo arranjo "por canal" precisa desse espacamento.

PUSHS
SECTION "SCL Variables", WRAM0

wSCL_SongPtr:           ds 2 ; ponteiro para a musica tocando agora (0 = nenhuma)
wSCL_PatternPtr:        ds 2 ; ponteiro para o padrao atual (referencia/depuracao)
wSCL_PatternListPos:    ds 2 ; posicao atual dentro da lista de padroes da musica
wSCL_ConductorChannel:  ds 1 ; canal da 1a faixa listada no padrao atual (decide quando avancar de padrao)
wSCL_CurrentTempo:      ds 1 ; ticks por linha "ao vivo" (copiado da musica, pode ser alterado por SCL_EFFECT_SET_TEMPO)
wSCL_TempoCounter:      ds 1
wSCL_EventDuration:     ds 1 ; duracao (em ticks) do evento sendo processado
wSCL_CurrentEffect:      ds 1
wSCL_CurrentEffectParam: ds 1
wSCL_TrackVolume:       ds 1
wSCL_TrackPan:          ds 1
wSCL_ChannelGate:       ds 1
wSCL_ChannelVelocity:   ds 1 ; velocidade de decaimento do envelope (0=usa a do instrumento, 1-15=sobrescreve)
wSCL_CPUPriority:       ds 1
wSCL_MasterVolume:      ds 1
wSCL_MasterPan:         ds 1
wSCL_RandomSeed:        ds 2
wSCL_CurrentWaveform:   ds 2 ; ponteiro da forma de onda carregada agora no wave RAM (0 = nenhuma)
wSCL_ScratchChannel:    ds 1 ; uso interno (numero do canal sendo processado)
wSCL_ScratchNoteLo:     ds 1 ; uso interno (SCL_PlayNote)
wSCL_ScratchNoteHi:     ds 1
wSCL_ScratchDuty:       ds 1
wSCL_ScratchEnv:        ds 1
wSCL_ScratchSweep:      ds 1
wSCL_ScratchLen:        ds 1
wSCL_ScratchWaveformLo: ds 1
wSCL_ScratchWaveformHi: ds 1
wSCL_ScratchNoise:      ds 1
wSCL_ScratchVolume:     ds 1
wSCL_ScratchPan:        ds 1

wSCL_ChannelVars_START:
wSCL_ChannelActive:        ds 8 ; 1 byte usado por canal (0/1)
wSCL_ChannelDuration:      ds 8 ; 1 byte usado por canal (ticks restantes)
wSCL_ChannelInstrument:    ds 8 ; word por canal (ponteiro para o instrumento)
wSCL_ChannelNote:          ds 8 ; word por canal (periodo tocando agora, ja com efeitos aplicados)
wSCL_ChannelRootNote:      ds 8 ; word por canal (periodo "raiz" da nota, sem efeitos - usado pelo arpejo)
wSCL_ChannelVolume:        ds 8 ; 1 byte usado por canal
wSCL_ChannelArp:           ds 8 ; 1 byte usado por canal (fase do arpejo: 0,1,2)
wSCL_ChannelSlideDest:     ds 8 ; word por canal (nota de destino do slide)
wSCL_ChannelSlideSpeed:    ds 8 ; 1 byte usado por canal (com sinal)
wSCL_ChannelVibratoPhase:  ds 8 ; 1 byte usado por canal
wSCL_ChannelTremoloPhase:  ds 8 ; 1 byte usado por canal
wSCL_TrackEventPtr:        ds 8 ; word por canal (ponteiro pro proximo evento da faixa; 0 = canal sem faixa neste padrao)
wSCL_ChannelVars_END:

wSCL_WaveformBuffer: ds 16 ; o wave RAM do Game Boy tem exatamente 16 bytes

POPS

; ============================================================
; MACROS
; ============================================================

; Macro: DEF_WAVEFORM
; \1 = nome. \2..\17 (opcional) = os 16 bytes crus da forma de onda
; (cada byte = 2 amostras de 4 bits - o mesmo tamanho do wave RAM real
; do Game Boy, $FF30-$FF3F). Sem os 16 bytes extras, usa uma onda
; triangular pronta. Para o canal 3 tocar formas de onda diferentes
; dependendo do instrumento (um "banco" de waveforms), passe o rotulo
; de uma DEF_WAVEFORM como 9o parametro de DEF_INSTRUMENT - o motor
; troca o conteudo do wave RAM sozinho quando necessario.
MACRO DEF_WAVEFORM
\1:
    if _NARG >= 17
        db \2, \3, \4, \5, \6, \7, \8, \9
        db \10, \11, \12, \13, \14, \15, \16, \17
    else
        db $01, $23, $45, $67, $89, $AB, $CD, $EF
        db $FE, $DC, $BA, $98, $76, $54, $32, $10
    endc
ENDM

; Macro: DEF_WAVEFORM_SAWTOOTH
; Forma de onda pronta: dente-de-serra (rampa ascendente, timbre mais
; brilhante/nasalado que a triangular padrao).
MACRO DEF_WAVEFORM_SAWTOOTH
\1:
    db $01, $23, $45, $67, $89, $AB, $CD, $EF
    db $01, $23, $45, $67, $89, $AB, $CD, $EF
ENDM

; Macro: DEF_WAVEFORM_SQUARE
; Forma de onda pronta: quadrada (metade em nivel alto, metade em
; nivel baixo - soa parecido com os canais de pulso, mas sem duty).
MACRO DEF_WAVEFORM_SQUARE
\1:
    db $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
    db $00, $00, $00, $00, $00, $00, $00, $00
ENDM

; Macro: DEF_INSTRUMENT
; \1 = nome, \2 = duty (0-3), \3 = envelope, \4 = sweep, \5 = tamanho (0-63)
; \6 = efeito continuo (opcional, padrao SCL_EFFECT_NONE) - ver SCL_EFFECT_*
; \7 = parametro do efeito (opcional, padrao 0). Para SCL_EFFECT_ARPEGGIO,
;      o nibble alto = semitons do 2o passo do arpejo, nibble baixo = 3o passo
;      (ex.: $47 = arpejo maior: raiz, +4, +7 semitons)
; \8 = configuracao de ruido para uso no canal 4 (opcional, padrao 0) -
;      combine com NOISE_PERIOD_x | NOISE_STEP_x
; \9 = forma de onda para uso no canal 3 (opcional, rotulo criado com
;      DEF_WAVEFORM/DEF_WAVEFORM_SAWTOOTH/etc.) - sem isso, o instrumento
;      usa a forma de onda que ja estiver carregada no wave RAM
MACRO DEF_INSTRUMENT
\1:
    db (\2 << 6) & $C0
    db \3
    db \4
    db \5
    if _NARG >= 9
        dw \9
    else
        dw 0
    endc
    if _NARG >= 8
        db \8
    else
        db 0
    endc
    if _NARG >= 6
        db \6
    else
        db 0
    endc
    if _NARG >= 7
        db \7
    else
        db 0
    endc
    db 15, 2, 1
ENDM

; Macro: SCL_LOAD_WAVEFORM
MACRO SCL_LOAD_WAVEFORM
    ld hl, \1
    ld de, rWavePattern
    ld bc, 16
    call SCL_CopyWaveform
ENDM

; Macro: PLAY_NOISE_ADVANCED
MACRO PLAY_NOISE_ADVANCED
    ld a, \3
    ld [rNR42], a
    ld a, \1
    ld [rNR43], a
    ld a, %10000000
    ld [rNR44], a
    ld a, \2
    call SCL_WaitTicks
    xor a
    ld [rNR42], a
ENDM

; Macro: PLAY_NOISE_DRUM
MACRO PLAY_NOISE_DRUM
    if strcmp("\1", "KICK") == 0
        PLAY_NOISE_ADVANCED NOISE_PERIOD_7 | NOISE_STEP_7BIT, \2, $F8
    elif strcmp("\1", "SNARE") == 0
        PLAY_NOISE_ADVANCED NOISE_PERIOD_4 | NOISE_STEP_15BIT, \2, $EA
    elif strcmp("\1", "HAT") == 0
        PLAY_NOISE_ADVANCED NOISE_PERIOD_1 | NOISE_STEP_7BIT, \2, $FA
    elif strcmp("\1", "CYMBAL") == 0
        PLAY_NOISE_ADVANCED NOISE_PERIOD_0 | NOISE_STEP_7BIT, \2, $F5
    elif strcmp("\1", "TOM") == 0
        PLAY_NOISE_ADVANCED NOISE_PERIOD_6 | NOISE_STEP_15BIT, \2, $F8
    endc
ENDM

; Macro: BEGIN_SONG
; \1 = nome da musica, \2 = tempo (ticks por linha), \3 = bpm (opcional, informativo)
MACRO BEGIN_SONG
\1:
    db \2
    dw \1_PatternList
    if _NARG >= 3
        db \3
    else
        db 0
    endc
\1_PatternList:
ENDM

; Macro: ADD_PATTERN
MACRO ADD_PATTERN
    dw \1
ENDM

; Macro: END_SONG
MACRO END_SONG
    dw $FFFF
ENDM

; Macro: BEGIN_PATTERN
; \1 = nome, \2 = numero de linhas (informativo)
; Um padrao agora contem uma LISTA de faixas (uma por canal usado) -
; declare cada uma com ADD_TRACK e feche com END_PATTERN. Exemplo:
;
;   BEGIN_PATTERN MyPattern, 16
;       ADD_TRACK 1, MelodyTrack
;       ADD_TRACK 2, BassTrack
;       ADD_TRACK 4, DrumTrack
;   END_PATTERN
;
;   BEGIN_TRACK MelodyTrack
;       NOTE C4, 10, Piano
;       END_TRACK
;   BEGIN_TRACK BassTrack
;       NOTE E0, 10, Bass
;       END_TRACK
;   BEGIN_TRACK DrumTrack
;       NOTE C4, 10, DrumKick
;       END_TRACK
MACRO BEGIN_PATTERN
\1:
    db \2
    dw \1_TrackList
\1_TrackList:
ENDM

; Macro: ADD_TRACK
; \1 = canal (1-4), \2 = rotulo da faixa (criado com BEGIN_TRACK)
MACRO ADD_TRACK
    db \1
    dw \2
ENDM

; Macro: END_PATTERN
MACRO END_PATTERN
    db 0 ; terminador da lista de faixas
ENDM

; Macro: BEGIN_TRACK
MACRO BEGIN_TRACK
\1:
ENDM

; Macro: NOTE
; \1 = nota (ex: C4), \2 = duracao em ticks, \3 = instrumento
; \4 = volume da nota (opcional, 0-15, padrao 15)
; \5 = gate (opcional, ticks a MENOS que a duracao total - 0 = toca a
;      nota inteira, valores maiores encurtam o final dela = staccato)
; \6 = velocity (opcional, 0-15, padrao 0 = nao mexe no instrumento) -
;      sobrescreve a VELOCIDADE DE DECAIMENTO do envelope (nao o
;      volume - isso e o campo \4). Um valor mais alto faz a nota
;      "murchar" mais devagar depois do ataque, independente de quao
;      alto ela comeca.
MACRO NOTE
    dw \1
    db \2
    dw \3
    db 0, 0
    if _NARG >= 4
        db \4
    else
        db 15
    endc
    db 2
    if _NARG >= 5
        db \5
    else
        db 0
    endc
    if _NARG >= 6
        db \6
    else
        db 0
    endc
ENDM

; Macro: REST
MACRO REST
    dw SCL_NOTE_REST
    db \1
    dw 0
    db 0, 0
    db 0, 0
    db 0, 0
ENDM

; Macro: END_TRACK
MACRO END_TRACK
    dw SCL_NOTE_END
    db 0
    dw 0
    db 0, 0
    db 0, 0
    db 0, 0
ENDM

; Macro: SET_TEMPO
; Comando de linha: muda o andamento da musica a partir deste ponto da
; faixa (\1 = novo numero de ticks por linha). So precisa ser usado em
; UMA faixa do padrao (normalmente a do canal "maestro").
MACRO SET_TEMPO
    dw SCL_NOTE_REST
    db 0
    dw 0
    db SCL_EFFECT_SET_TEMPO, \1
    db 0, 2, 0, 0
ENDM

; Macro: PLAY_NOTE_CH1
MACRO PLAY_NOTE_CH1
    ld a, LOW(\1)
    ld [rNR13], a
    ld a, HIGH(\1)
    or %10000000
    ld [rNR14], a
    ld a, \2
    call SCL_WaitTicks
    xor a
    ld [rNR12], a
ENDM

; Macro: PLAY_NOTE_CH2
MACRO PLAY_NOTE_CH2
    ld a, LOW(\1)
    ld [rNR23], a
    ld a, HIGH(\1)
    or %10000000
    ld [rNR24], a
    ld a, \2
    call SCL_WaitTicks
    xor a
    ld [rNR22], a
ENDM

; Macro: PLAY_NOTE_CH3
MACRO PLAY_NOTE_CH3
    ld a, LOW(\1)
    ld [rNR33], a
    ld a, HIGH(\1)
    or %10000000
    ld [rNR34], a
    ld a, \2
    call SCL_WaitTicks
    xor a
    ld [rNR30], a
ENDM

; Macro: PLAY_NOISE
MACRO PLAY_NOISE
    ld a, \3
    ld [rNR42], a
    ld a, \1
    ld [rNR43], a
    ld a, %10000000
    ld [rNR44], a
    ld a, \2
    call SCL_WaitTicks
    xor a
    ld [rNR42], a
ENDM

; Macro: SCL_SET_CPU_PRIORITY
MACRO SCL_SET_CPU_PRIORITY
    ld a, \1
    ld [wSCL_CPUPriority], a
ENDM

; Macro: SCL_ENABLE_INTERRUPTS
MACRO SCL_ENABLE_INTERRUPTS
    ld a, %00000001
    ld [rIE], a
    ei
ENDM

; Macro: SCL_DISABLE_INTERRUPTS
MACRO SCL_DISABLE_INTERRUPTS
    di
    xor a
    ld [rIE], a
ENDM

; Macro: SCL_SET_MASTER_VOLUME
MACRO SCL_SET_MASTER_VOLUME
    ld a, (\2 << 4) | \1
    ld [rNR50], a
    ld [wSCL_MasterVolume], a
ENDM

; Macro: SCL_SET_MASTER_PAN
MACRO SCL_SET_MASTER_PAN
    ld a, \1
    ld [rNR51], a
    ld [wSCL_MasterPan], a
ENDM

; Macro: SCL_SET_CHANNEL_VOLUME
MACRO SCL_SET_CHANNEL_VOLUME
    ld a, \2
    ld b, a
    ld a, \1
    call SCL_UpdateVolume
ENDM

; Macro: SCL_INIT
MACRO SCL_INIT
    ld a, $80
    ld [rNR52], a
    ld a, $FF
    ld [rNR51], a
    ld a, $77
    ld [rNR50], a
    xor a
    ld [wSCL_SongPtr], a
    ld [wSCL_SongPtr + 1], a
    ld [wSCL_PatternPtr], a
    ld [wSCL_PatternPtr + 1], a
    ld [wSCL_PatternListPos], a
    ld [wSCL_PatternListPos + 1], a
    ld [wSCL_ConductorChannel], a
    ld [wSCL_CurrentTempo], a
    ld [wSCL_TempoCounter], a
    ld [wSCL_EventDuration], a
    ld [wSCL_CurrentWaveform], a
    ld [wSCL_CurrentWaveform + 1], a
    ld hl, wSCL_ChannelVars_START
    ld bc, wSCL_ChannelVars_END - wSCL_ChannelVars_START
    call SCL_ClearMemory
    ld hl, wSCL_WaveformBuffer
    ld bc, 16
    call SCL_ClearMemory
    ld a, LOW($1234)
    ld [wSCL_RandomSeed], a
    ld a, HIGH($1234)
    ld [wSCL_RandomSeed + 1], a
    ld a, 2
    ld [wSCL_CPUPriority], a
    ld a, $77
    ld [wSCL_MasterVolume], a
    ld a, $FF
    ld [wSCL_MasterPan], a
ENDM

; ============================================================
; CODIGO DA ENGINE
; ============================================================

PUSHS

SECTION "SCL Code", ROM0

SCL_WaitTicks:
    push af
    push bc
    ld b, a
.wait_loop:
    call SCL_WaitFrame
    dec b
    jr nz, .wait_loop
    pop bc
    pop af
    ret

SCL_WaitFrame:
    ld a, [rLY]
    cp 144
    jr nz, SCL_WaitFrame
.wait_vblank_end:
    ld a, [rLY]
    cp 144
    jr z, .wait_vblank_end
    ret

; hl = origem, bc = quantidade de bytes, zera tudo
SCL_ClearMemory:
    push af
    push bc
    push hl
.clear_loop:
    xor a
    ld [hl+], a
    dec bc
    ld a, b
    or c
    jr nz, .clear_loop
    pop hl
    pop bc
    pop af
    ret

SCL_CopyWaveform:
    push af
    push hl
    push de
    push bc
    xor a
    ld [rNR30], a
.copy_loop:
    ld a, [hl+]
    ld [de], a
    inc de
    dec bc
    ld a, b
    or c
    jr nz, .copy_loop
    ld a, $80
    ld [rNR30], a
    pop bc
    pop de
    pop hl
    pop af
    ret

; hl = endereco da musica (rotulo criado por BEGIN_SONG)
SCL_PlaySong:
    ld a, l
    ld [wSCL_SongPtr], a
    ld a, h
    ld [wSCL_SongPtr + 1], a
    ld a, [hl]
    ld [wSCL_CurrentTempo], a
    inc hl                    ; pula o byte do tempo (offset 0)
    ld a, [hl+]
    ld [wSCL_PatternListPos], a
    ld a, [hl]
    ld [wSCL_PatternListPos + 1], a
    call SCL_AdvancePattern
    ret

; Le a entrada atual de wSCL_PatternListPos (word = endereco de um
; padrao), avanca o ponteiro da lista, e carrega esse padrao. Se a
; entrada for o terminador $FFFF (gravado por END_SONG), volta para o
; comeco da lista de padroes, fazendo a musica repetir em loop.
SCL_AdvancePattern:
    push af
    push bc
    push de
    push hl
    ld a, [wSCL_PatternListPos]
    ld l, a
    ld a, [wSCL_PatternListPos + 1]
    ld h, a
    ld a, [hl+]
    ld e, a
    ld a, [hl+]
    ld d, a
    ld a, d
    cp HIGH($FFFF)
    jr nz, .valid
    ld a, e
    cp LOW($FFFF)
    jr nz, .valid
    ; fim da lista: volta para o comeco (loop da musica)
    ld a, [wSCL_SongPtr]
    ld l, a
    ld a, [wSCL_SongPtr + 1]
    ld h, a
    inc hl
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
    ld l, e
    ld h, d
    ld a, [hl+]
    ld e, a
    ld a, [hl+]
    ld d, a
.valid:
    ld a, l
    ld [wSCL_PatternListPos], a
    ld a, h
    ld [wSCL_PatternListPos + 1], a
    call SCL_LoadPattern
    pop hl
    pop de
    pop bc
    pop af
    ret

; de = endereco de um padrao (rotulo criado por BEGIN_PATTERN)
; Percorre a lista de faixas do padrao (ADD_TRACK ... END_PATTERN) e
; associa cada faixa ao seu canal (wSCL_TrackEventPtr). A primeira
; faixa listada vira o canal "maestro" (wSCL_ConductorChannel): quando
; a faixa dele terminar, a musica avanca para o proximo padrao. Canais
; sem faixa neste padrao sao silenciados (evita nota presa de um
; padrao anterior).
SCL_LoadPattern:
    push af
    push bc
    push de
    push hl
    ld a, e
    ld [wSCL_PatternPtr], a
    ld a, d
    ld [wSCL_PatternPtr + 1], a
    ld hl, wSCL_TrackEventPtr
    ld b, 8
.clear_tracks:
    xor a
    ld [hl+], a
    dec b
    jr nz, .clear_tracks
    xor a
    ld [wSCL_ConductorChannel], a
    ld l, e
    ld h, d
    inc hl                    ; pula o byte de tamanho (offset 0)
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a                   ; de = endereco da lista de faixas
    ld l, e
    ld h, d
.track_loop:
    ld a, [hl+]
    or a
    jr z, .tracks_done         ; canal 0 = fim da lista
    ld c, a                     ; c = canal desta faixa
    ld a, [wSCL_ConductorChannel]
    or a
    jr nz, .not_first
    ld a, c
    ld [wSCL_ConductorChannel], a
.not_first:
    ld a, [hl+]
    ld e, a
    ld a, [hl+]
    ld d, a                    ; de = ponteiro dos eventos desta faixa
    push hl
    ld a, c
    dec a
    sla a
    ld hl, wSCL_TrackEventPtr
    add l
    ld l, a
    ld a, e
    ld [hl+], a
    ld a, d
    ld [hl], a
    ld a, c
    call SCL_StopChannel
    pop hl
    jr .track_loop
.tracks_done:
    xor a
    ld [wSCL_TempoCounter], a
    ld [wSCL_EventDuration], a
    pop hl
    pop de
    pop bc
    pop af
    ret

SCL_Update:
    push af
    push bc
    push de
    push hl
    ld a, [wSCL_SongPtr]
    ld b, a
    ld a, [wSCL_SongPtr + 1]
    or b
    jr z, .update_end
    ld a, [wSCL_TempoCounter]
    inc a
    ld [wSCL_TempoCounter], a
    ld b, a
    ld a, [wSCL_CurrentTempo]
    cp b
    jr nz, .process_channels
    xor a
    ld [wSCL_TempoCounter], a
    ld a, 1
    call SCL_ProcessNextEventForChannel
    ld a, 2
    call SCL_ProcessNextEventForChannel
    ld a, 3
    call SCL_ProcessNextEventForChannel
    ld a, 4
    call SCL_ProcessNextEventForChannel
.process_channels:
    ld a, 1
    call SCL_ProcessChannel
    ld a, 2
    call SCL_ProcessChannel
    ld a, 3
    call SCL_ProcessChannel
    ld a, 4
    call SCL_ProcessChannel
.update_end:
    pop hl
    pop de
    pop bc
    pop af
    ret

SCL_ProcessChannel:
    push af
    push bc
    push de
    push hl
    dec a
    sla a
    ld b, a                    ; b = indice do canal (0, 2, 4 ou 6)
    ld hl, wSCL_ChannelActive
    add l
    ld l, a
    ld a, [hl]
    or a
    jr z, .channel_inactive
    ld hl, wSCL_ChannelDuration
    ld a, b
    add l
    ld l, a
    ld a, [hl]
    dec a
    ld [hl], a
    jr nz, .check_effects
    ld a, b
    srl a
    inc a
    call SCL_StopChannel
    jr .channel_done
.check_effects:
    ld a, b
    srl a
    inc a
    call SCL_ProcessChannelEffects
.channel_inactive:
.channel_done:
    pop hl
    pop de
    pop bc
    pop af
    ret

; a = canal (1-4)
SCL_StopChannel:
    push af
    push bc
    push hl
    ld b, a
    dec a
    sla a
    ld hl, wSCL_ChannelActive
    add l
    ld l, a
    xor a
    ld [hl], a
    ld a, b
    cp 1
    jr z, .stop_ch1
    cp 2
    jr z, .stop_ch2
    cp 3
    jr z, .stop_ch3
    cp 4
    jr z, .stop_ch4
    jr .stop_done
.stop_ch1:
    xor a
    ld [rNR12], a
    jr .stop_done
.stop_ch2:
    xor a
    ld [rNR22], a
    jr .stop_done
.stop_ch3:
    xor a
    ld [rNR30], a
    jr .stop_done
.stop_ch4:
    xor a
    ld [rNR42], a
.stop_done:
    pop hl
    pop bc
    pop af
    ret

; a = canal (1-4)
SCL_ProcessChannelEffects:
    push af
    push bc
    push de
    push hl
    ld [wSCL_ScratchChannel], a
    dec a
    sla a
    ld hl, wSCL_ChannelInstrument
    add l
    ld l, a
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
    ld a, e
    or d
    jr z, .no_effect
    ld hl, SCL_Instrument_effect
    add hl, de
    ld a, [hl+]
    ld b, a
    ld c, [hl]
    ld a, b
    or a
    jr z, .no_effect
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelNote
    add l
    ld l, a
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
    ld a, [wSCL_ScratchChannel]
    call SCL_ApplyEffect
.no_effect:
    pop hl
    pop de
    pop bc
    pop af
    ret

; de = periodo a procurar. Retorna o indice na SCL_NoteTable em "a",
; ou $FF se a nota nao estiver na tabela (ex.: percussao/ruido).
SCL_FindNoteIndex:
    push bc
    push de
    push hl
    ld hl, SCL_NoteTable
    ld b, 0
.search:
    ld a, b
    cp SCL_NoteTable_LEN
    jr z, .not_found
    ld a, [hl+]
    ld c, a
    ld a, [hl+]
    cp d
    jr nz, .next
    ld a, c
    cp e
    jr z, .found
.next:
    inc b
    jr .search
.found:
    ld a, b
    jr .done
.not_found:
    ld a, $FF
.done:
    pop hl
    pop de
    pop bc
    ret

; a = indice na SCL_NoteTable (0 a SCL_NoteTable_LEN-1). Retorna o
; periodo correspondente em "de".
SCL_NoteFromIndex:
    push bc
    push hl
    ld c, a
    ld b, 0
    sla c
    rl b                       ; bc = indice*2 (deslocamento em bytes)
    ld hl, SCL_NoteTable
    add hl, bc
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
    pop hl
    pop bc
    ret

; a = canal (1-4), b = efeito, c = parametro do efeito, de = nota atual (word)
SCL_ApplyEffect:
    push af
    push bc
    push de
    push hl
    ld [wSCL_ScratchChannel], a
    ld a, b
    cp SCL_EFFECT_ARPEGGIO
    jp z, .arp
    cp SCL_EFFECT_SLIDE
    jp z, .slide
    cp SCL_EFFECT_VIBRATO
    jp z, .vibrato
    cp SCL_EFFECT_TREMOLO
    jp z, .tremolo
    jp .effect_done
.arp:
    ; avanca a fase do arpejo: 0 = nota raiz, 1 e 2 = passos do acorde
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelArp
    add l
    ld l, a
    ld a, [hl]
    inc a
    and 3
    cp 3
    jr nz, .arp_phase_ok
    xor a
.arp_phase_ok:
    ld [hl], a
    or a
    jr z, .arp_use_root
    ; escolhe o deslocamento em semitons: fase1 = nibble alto do
    ; parametro do efeito, fase2 = nibble baixo (ex.: $47 = +4, +7)
    dec a
    jr nz, .arp_low_nibble
    ld a, c
    swap a
    and $0F
    jr .arp_have_offset
.arp_low_nibble:
    ld a, c
    and $0F
.arp_have_offset:
    ld b, a
    ; acha o indice da nota RAIZ (nao a nota ja modulada) na tabela
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelRootNote
    add l
    ld l, a
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
    call SCL_FindNoteIndex
    cp $FF
    jr z, .arp_use_root
    add b
    cp SCL_NoteTable_LEN
    jr c, .arp_index_ok
    ld a, SCL_NoteTable_LEN - 1  ; satura no topo da tabela
.arp_index_ok:
    call SCL_NoteFromIndex        ; de = periodo do passo do arpejo
    jr .arp_apply
.arp_use_root:
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelRootNote
    add l
    ld l, a
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
.arp_apply:
    ld a, [wSCL_ScratchChannel]
    call SCL_UpdateNote
    jp .effect_done
.slide:
    push de
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelSlideDest
    add l
    ld l, a
    ld a, [hl+]
    ld b, a
    ld a, [hl]
    ld c, a                    ; bc = nota de destino
    pop de                     ; de = nota atual
    ld a, e
    cp b
    jr nz, .slide_diff
    ld a, d
    cp c
    jr z, .slide_done
.slide_diff:
    push bc
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelSlideSpeed
    add l
    ld l, a
    ld a, [hl]                 ; velocidade do slide (com sinal)
    ld l, a
    ld h, 0
    bit 7, a
    jr z, .slide_pos
    ld h, $FF                  ; extensao de sinal (velocidade negativa)
.slide_pos:
    add hl, de
    ld d, h
    ld e, l
    pop bc
    ld a, [wSCL_ScratchChannel]
    call SCL_UpdateNote
.slide_done:
    jp .effect_done
.vibrato:
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelVibratoPhase
    add l
    ld l, a
    ld a, [hl]
    add c
    ld [hl], a
    sra a
    sra a
    ld l, a
    ld h, 0
    bit 7, a
    jr z, .vib_pos
    ld h, $FF
.vib_pos:
    add hl, de
    ld d, h
    ld e, l
    ld a, [wSCL_ScratchChannel]
    call SCL_UpdateNote
    jp .effect_done
.tremolo:
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelTremoloPhase
    add l
    ld l, a
    ld a, [hl]
    add c
    ld [hl], a
    sra a
    sra a
    and $0F
    ld b, a
    ld a, [wSCL_ScratchChannel]
    call SCL_UpdateVolume
.effect_done:
    pop hl
    pop de
    pop bc
    pop af
    ret

; a = canal (1-4), de = nova nota (word, periodo de 11 bits)
SCL_UpdateNote:
    push af
    push bc
    push de
    push hl
    ld c, a
    dec a
    sla a
    ld hl, wSCL_ChannelNote
    add l
    ld l, a
    ld a, e
    ld [hl+], a
    ld a, d
    ld [hl], a
    ld a, c
    cp 1
    jr z, .update_ch1
    cp 2
    jr z, .update_ch2
    cp 3
    jr z, .update_ch3
    jr .update_done
.update_ch1:
    ld a, e
    ld [rNR13], a
    ld a, d
    and $07
    or %10000000
    ld [rNR14], a
    jr .update_done
.update_ch2:
    ld a, e
    ld [rNR23], a
    ld a, d
    and $07
    or %10000000
    ld [rNR24], a
    jr .update_done
.update_ch3:
    ld a, e
    ld [rNR33], a
    ld a, d
    and $07
    or %10000000
    ld [rNR34], a
.update_done:
    pop hl
    pop de
    pop bc
    pop af
    ret

; a = canal (1-4), b = volume (0-15)
SCL_UpdateVolume:
    push af
    push bc
    push hl
    ld c, a
    dec a
    sla a
    ld hl, wSCL_ChannelVolume
    add l
    ld l, a
    ld a, b
    ld [hl], a
    ld a, c
    cp 1
    jr z, .vol_ch1
    cp 2
    jr z, .vol_ch2
    cp 3
    jr z, .vol_ch3
    cp 4
    jr z, .vol_ch4
    jr .vol_done
.vol_ch1:
    ld a, [rNR12]
    and $0F
    or b
    ld [rNR12], a
    jr .vol_done
.vol_ch2:
    ld a, [rNR22]
    and $0F
    or b
    ld [rNR22], a
    jr .vol_done
.vol_ch3:
    ; o canal 3 (onda) so aceita 0/100%/50%/25% (bits 6-5 de NR32) -
    ; converte o volume 0-15 para a faixa mais proxima.
    ld a, b
    cp 0
    jr z, .vol_ch3_mute
    cp 8
    jr nc, .vol_ch3_full
    cp 4
    jr nc, .vol_ch3_half
    ld a, %01100000
    jr .vol_ch3_apply
.vol_ch3_half:
    ld a, %01000000
    jr .vol_ch3_apply
.vol_ch3_full:
    ld a, %00100000
    jr .vol_ch3_apply
.vol_ch3_mute:
    xor a
.vol_ch3_apply:
    ld [rNR32], a
    jr .vol_done
.vol_ch4:
    ld a, [rNR42]
    and $0F
    or b
    ld [rNR42], a
.vol_done:
    pop hl
    pop bc
    pop af
    ret

; a = canal (1-4)
; Le o proximo evento da faixa deste canal (wSCL_TrackEventPtr) e o
; toca. Se o canal nao tem faixa neste padrao (ponteiro = 0), nao faz
; nada. Ao chegar no fim da faixa: se este e o canal "maestro"
; (wSCL_ConductorChannel), avanca a musica para o proximo padrao;
; senao, so silencia este canal ate o proximo padrao carregar.
SCL_ProcessNextEventForChannel:
    push hl
    push de
    push bc
    push af
    ld [wSCL_ScratchChannel], a
    dec a
    sla a
    ld hl, wSCL_TrackEventPtr
    add l
    ld l, a
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
    ld a, e
    or d
    jr z, .no_track
    ld l, e
    ld h, d
    ld a, [hl+]
    ld c, a
    ld a, [hl+]
    ld b, a                    ; bc = nota
    ld a, b
    cp HIGH(SCL_NOTE_END)
    jr nz, .not_end
    ld a, c
    cp LOW(SCL_NOTE_END)
    jr z, .end_of_track
.not_end:
    ld a, [hl+]
    ld [wSCL_EventDuration], a
    ld a, [hl+]
    ld e, a
    ld a, [hl+]
    ld d, a                    ; de = ponteiro do instrumento
    ld a, [hl+]
    ld [wSCL_CurrentEffect], a
    ld a, [hl+]
    ld [wSCL_CurrentEffectParam], a
    ld a, [wSCL_CurrentEffect]
    cp SCL_EFFECT_SET_TEMPO
    jr nz, .not_tempo_cmd
    ld a, [wSCL_CurrentEffectParam]
    ld [wSCL_CurrentTempo], a
.not_tempo_cmd:
    ld a, [hl+]
    ld [wSCL_TrackVolume], a
    ld a, [hl+]
    ld [wSCL_TrackPan], a
    ld a, [hl+]
    ld [wSCL_ChannelGate], a
    ld a, [hl+]
    ld [wSCL_ChannelVelocity], a
    ; guarda nota (bc) e instrumento (de), grava a nova posicao (hl) de
    ; volta no slot deste canal, depois recupera nota/instrumento
    push bc
    push de
    ld d, h
    ld e, l
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_TrackEventPtr
    add l
    ld l, a
    ld a, e
    ld [hl+], a
    ld a, d
    ld [hl], a
    pop de
    pop bc
    ld a, [wSCL_ScratchChannel]
    call SCL_PlayNote
    jr .done
.no_track:
    jr .done
.end_of_track:
    ld a, [wSCL_ScratchChannel]
    ld b, a
    ld a, [wSCL_ConductorChannel]
    cp b
    jr nz, .stop_this_track
    call SCL_AdvancePattern
    jr .done
.stop_this_track:
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_TrackEventPtr
    add l
    ld l, a
    xor a
    ld [hl+], a
    ld [hl], a
    ld a, [wSCL_ScratchChannel]
    call SCL_StopChannel
.done:
    pop af
    pop bc
    pop de
    pop hl
    ret

; a = canal (1-4), bc = nota (word, periodo de 11 bits, ou SCL_NOTE_REST)
; de = ponteiro do instrumento
; Usa wSCL_EventDuration e wSCL_ChannelGate (ja definidos pelo
; chamador) para calcular a duracao efetiva da nota, e wSCL_TrackVolume
; para o volume. Programa o canal de hardware correto (1-4) a partir
; dos dados do instrumento, incluindo o pan (NR51).
SCL_PlayNote:
    push af
    push bc
    push de
    push hl
    ld [wSCL_ScratchChannel], a
    ld a, b
    cp HIGH(SCL_NOTE_REST)
    jr nz, .play
    ld a, c
    cp LOW(SCL_NOTE_REST)
    jp z, .done
.play:
    ld a, c
    ld [wSCL_ScratchNoteLo], a
    ld a, b
    ld [wSCL_ScratchNoteHi], a
    ; le os campos do instrumento
    ld l, e
    ld h, d
    ld a, [hl+]
    ld [wSCL_ScratchDuty], a
    ld a, [hl+]
    ld [wSCL_ScratchEnv], a
    ld a, [hl+]
    ld [wSCL_ScratchSweep], a
    ld a, [hl+]
    ld [wSCL_ScratchLen], a
    ld a, [hl+]
    ld [wSCL_ScratchWaveformLo], a
    ld a, [hl+]
    ld [wSCL_ScratchWaveformHi], a
    ld a, [hl+]
    ld [wSCL_ScratchNoise], a  ; +6
    inc hl
    inc hl                     ; pula effect(+7) e effect_param(+8) do instrumento
    ld a, [hl+]
    ld [wSCL_ScratchVolume], a ; +9 (reservado; ch1/2/4 usam TrackVolume)
    ld a, [hl]
    ld [wSCL_ScratchPan], a    ; +10
    ; combina o volume da nota (wSCL_TrackVolume, 0-15) com a
    ; direcao/velocidade do envelope do instrumento
    ld a, [wSCL_ScratchEnv]
    and $0F
    ld b, a
    ld a, [wSCL_TrackVolume]
    swap a
    or b
    ld [wSCL_ScratchEnv], a
    ; velocity (0-15, opcional): sobrescreve a VELOCIDADE DE DECAIMENTO
    ; do envelope (bits 2-0 de NRx2), mantendo o volume/direcao que
    ; acabaram de ser combinados acima. velocity=0 (padrao) nao mexe
    ; em nada - usa o decaimento original do instrumento.
    ld a, [wSCL_ChannelVelocity]
    or a
    jr z, .velocity_done
    srl a                       ; 0-15 -> 0-7 (campo tem so 3 bits)
    ld b, a
    ld a, [wSCL_ScratchEnv]
    and $F8
    or b
    ld [wSCL_ScratchEnv], a
.velocity_done:
    ; atualiza o estado do canal (raiz/nota/instrumento/ativo)
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelNote
    add l
    ld l, a
    ld a, [wSCL_ScratchNoteLo]
    ld [hl+], a
    ld a, [wSCL_ScratchNoteHi]
    ld [hl], a
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelRootNote
    add l
    ld l, a
    ld a, [wSCL_ScratchNoteLo]
    ld [hl+], a
    ld a, [wSCL_ScratchNoteHi]
    ld [hl], a
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelInstrument
    add l
    ld l, a
    ld a, e
    ld [hl+], a
    ld a, d
    ld [hl], a
    ; duracao efetiva = duracao do evento - gate (staccato), minimo 1 tick
    ld a, [wSCL_EventDuration]
    ld b, a
    ld a, [wSCL_ChannelGate]
    ld c, a
    ld a, b
    sub c
    jr nc, .gate_ok
    ld a, 1
    jr .gate_store
.gate_ok:
    or a
    jr nz, .gate_store
    ld a, 1
.gate_store:
    ld b, a
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelDuration
    add l
    ld l, a
    ld a, b
    ld [hl], a
    ld a, [wSCL_ScratchChannel]
    dec a
    sla a
    ld hl, wSCL_ChannelActive
    add l
    ld l, a
    ld a, 1
    ld [hl], a
    ; programa o hardware do canal certo
    ld a, [wSCL_ScratchChannel]
    cp 1
    jp z, .hw_ch1
    cp 2
    jp z, .hw_ch2
    cp 3
    jp z, .hw_ch3
    cp 4
    jp z, .hw_ch4
    jp .after_hw
.hw_ch1:
    ld a, [wSCL_ScratchEnv]
    ld [rNR12], a
    ld a, [wSCL_ScratchSweep]
    ld [rNR10], a
    ld a, [wSCL_ScratchDuty]
    and $C0
    ld b, a
    ld a, [wSCL_ScratchLen]
    and $3F
    or b
    ld [rNR11], a
    ld a, [wSCL_ScratchNoteLo]
    ld [rNR13], a
    ld a, [wSCL_ScratchNoteHi]
    and $07
    or %10000000
    ld [rNR14], a
    jp .after_hw
.hw_ch2:
    ld a, [wSCL_ScratchEnv]
    ld [rNR22], a
    ld a, [wSCL_ScratchDuty]
    and $C0
    ld b, a
    ld a, [wSCL_ScratchLen]
    and $3F
    or b
    ld [rNR21], a
    ld a, [wSCL_ScratchNoteLo]
    ld [rNR23], a
    ld a, [wSCL_ScratchNoteHi]
    and $07
    or %10000000
    ld [rNR24], a
    jp .after_hw
.hw_ch3:
    ; se o instrumento tem sua propria forma de onda e ela e diferente
    ; da que ja esta carregada, troca o conteudo do wave RAM agora.
    ; Sem forma de onda propria (waveform=0), mantem o que ja estiver
    ; la (ex.: carregado manualmente via SCL_LOAD_WAVEFORM).
    ld a, [wSCL_ScratchWaveformLo]
    ld b, a
    ld a, [wSCL_ScratchWaveformHi]
    ld c, a
    ld a, b
    or c
    jr z, .ch3_skip_wave_load
    ld a, [wSCL_CurrentWaveform]
    cp b
    jr nz, .ch3_do_wave_load
    ld a, [wSCL_CurrentWaveform + 1]
    cp c
    jr z, .ch3_skip_wave_load
.ch3_do_wave_load:
    ld l, b
    ld h, c
    ld de, rWavePattern
    push bc
    ld bc, 16
    call SCL_CopyWaveform        ; ja desliga/religa o DAC durante a copia
    pop bc
    ld a, b
    ld [wSCL_CurrentWaveform], a
    ld a, c
    ld [wSCL_CurrentWaveform + 1], a
.ch3_skip_wave_load:
    xor a
    ld [rNR30], a               ; desliga o DAC antes de trocar volume/nota (evita estalo)
    ld a, [wSCL_TrackVolume]
    cp 0
    jr z, .ch3_mute
    cp 8
    jr nc, .ch3_full
    cp 4
    jr nc, .ch3_half
    ld a, %01100000
    jr .ch3_apply
.ch3_half:
    ld a, %01000000
    jr .ch3_apply
.ch3_full:
    ld a, %00100000
    jr .ch3_apply
.ch3_mute:
    xor a
.ch3_apply:
    ld [rNR32], a
    ld a, $80
    ld [rNR30], a
    ld a, [wSCL_ScratchNoteLo]
    ld [rNR33], a
    ld a, [wSCL_ScratchNoteHi]
    and $07
    or %10000000
    ld [rNR34], a
    jp .after_hw
.hw_ch4:
    ld a, [wSCL_ScratchEnv]
    ld [rNR42], a
    ld a, [wSCL_ScratchNoise]
    ld [rNR43], a
    ld a, %10000000
    ld [rNR44], a
.after_hw:
    ld a, [wSCL_ScratchPan]
    ld b, a
    ld a, [wSCL_ScratchChannel]
    dec a
    call SCL_ApplyChannelPan
.done:
    pop hl
    pop de
    pop bc
    pop af
    ret

; a = indice do canal (0-3), b = pan (SCL_PAN_LEFT/RIGHT/MUTE/CENTER)
; Liga/desliga so os 2 bits deste canal em NR51, preservando o pan dos
; outros 3 canais - permite pan diferente por instrumento de verdade.
SCL_ApplyChannelPan:
    push bc
    push de
    push hl
    ld c, a
    ld hl, SCL_ChannelPanMaskRight
    add l
    ld l, a
    ld a, [hl]
    ld d, a                     ; d = mascara direita deste canal
    ld hl, SCL_ChannelPanMaskLeft
    ld a, c
    add l
    ld l, a
    ld a, [hl]
    ld e, a                     ; e = mascara esquerda deste canal
    ld a, d
    or e
    cpl
    ld h, a                     ; h = mascara de limpeza (NOT dos 2 bits)
    ld a, [rNR51]
    and h
    ld h, a                     ; h = NR51 com os bits deste canal zerados
    ld a, b
    cp SCL_PAN_LEFT
    jr z, .pan_left
    cp SCL_PAN_RIGHT
    jr z, .pan_right
    cp SCL_PAN_CENTER
    jr z, .pan_center
.pan_mute:
    ld a, h
    jr .apply
.pan_left:
    ld a, h
    or e
    jr .apply
.pan_right:
    ld a, h
    or d
    jr .apply
.pan_center:
    ld a, h
    or d
    or e
.apply:
    ld [rNR51], a
    pop hl
    pop de
    pop bc
    ret

SCL_ChannelPanMaskRight:
    db %00000001, %00000010, %00000100, %00001000
SCL_ChannelPanMaskLeft:
    db %00010000, %00100000, %01000000, %10000000

; Tabela de periodos em ordem crescente de altura (do grave pro agudo),
; usada pelo efeito de arpejo pra andar em semitons musicais reais em
; vez de somar um numero cru ao periodo (que nao e linear no hardware).
SCL_NoteTable:
    dw C0, C0s, D0, D0s, E0, F0, F0s, G0, G0s, A0, A0s, B0
    dw C1, C1s, D1, D1s, E1, F1, F1s, G1, G1s, A1, A1s, B1
    dw C2, C2s, D2, D2s, E2, F2, F2s, G2, G2s, A2, A2s, B2
    dw C3, C3s, D3, D3s, E3, F3, F3s, G3, G3s, A3, A3s, B3
    dw C4, C4s, D4, D4s
def SCL_NoteTable_LEN equ 52

; Retorna em "a" um bitmask com os canais ativos agora (bit0=canal1,
; bit1=canal2, bit2=canal3, bit3=canal4). Util para depuracao (VU
; meter simples, contador de vozes).
SCL_GetActiveVoices:
    push bc
    push hl
    ld b, 0
    ld hl, wSCL_ChannelActive
    ld a, [hl]
    or a
    jr z, .c2
    set 0, b
.c2:
    inc hl
    inc hl
    ld a, [hl]
    or a
    jr z, .c3
    set 1, b
.c3:
    inc hl
    inc hl
    ld a, [hl]
    or a
    jr z, .c4
    set 2, b
.c4:
    inc hl
    inc hl
    ld a, [hl]
    or a
    jr z, .gv_done
    set 3, b
.gv_done:
    ld a, b
    pop hl
    pop bc
    ret

; CORRIGIDO (v20): gerador pseudo-aleatorio de 16 bits (LFSR de Galois).
SCL_Random:
    push hl
    push de
    push bc
    ld hl, wSCL_RandomSeed
    ld a, [hl+]
    ld e, a
    ld a, [hl]
    ld d, a
    ld a, e
    and $01
    ld c, a
    srl d
    rr e
    ld a, c
    or a
    jr z, .no_tap
    ld a, e
    xor LOW($B400)
    ld e, a
    ld a, d
    xor HIGH($B400)
    ld d, a
.no_tap:
    ld a, d
    ld [wSCL_RandomSeed + 1], a
    ld a, e
    ld [wSCL_RandomSeed], a
    pop bc
    pop de
    pop hl
    ret

; b = 0/1/3 (esquerda/direita/centro), qualquer outro valor = ambos.
; Controle rapido de TODOS os canais de uma vez (para pan por
; instrumento, use SCL_ApplyChannelPan, chamado automaticamente por
; SCL_PlayNote a partir do campo "pan" do instrumento).
SCL_SetChannelPan:
    push af
    push hl
    ld a, b
    cp 0
    jr z, .pan_left
    cp 1
    jr z, .pan_right
    cp 3
    jr z, .pan_center
.pan_both:
    ld a, $FF
    jr .apply_pan
.pan_left:
    ld a, $0F
    jr .apply_pan
.pan_right:
    ld a, $F0
    jr .apply_pan
.pan_center:
    ld a, $AA
.apply_pan:
    ld [rNR51], a
    pop hl
    pop af
    ret

POPS

; ============================================================
; DADOS EM ROM
; ============================================================

PUSHS

SECTION "SCL Instruments", ROM0

; Formas de onda para o canal 3 (banco de waveforms). Um instrumento
; que referencia uma destas via DEF_INSTRUMENT faz o motor trocar o
; wave RAM sozinho quando necessario - so recarrega quando a forma de
; onda realmente muda de uma nota pra outra.
DEF_WAVEFORM WaveWarm            ; onda triangular padrao (soa suave/quente)
DEF_WAVEFORM_SAWTOOTH WaveBright ; dente-de-serra (mais brilhante/nasalado)
DEF_WAVEFORM_SQUARE WaveBuzzy    ; quadrada (mais crua/zumbida)

; Instrumentos pre-definidos. Synth mostra o vibrato continuo
; configuravel via DEF_INSTRUMENT; os quatro tambores mostram a
; configuracao de ruido usada quando tocados pelo sequenciador
; (canal 4) em vez do macro PLAY_NOISE_DRUM; os tres "Wave*" mostram o
; banco de formas de onda do canal 3.
DEF_INSTRUMENT Piano, %10, $F2, 0, 0
DEF_INSTRUMENT Guitar, %01, $E5, 0, 0
DEF_INSTRUMENT Bass, %11, $F8, 0, 0
DEF_INSTRUMENT Flute, %00, $F0, 0, 0
DEF_INSTRUMENT Synth, %10, $FA, 1, 0, SCL_EFFECT_VIBRATO, 4
DEF_INSTRUMENT DrumKick,   %00, $F8, 0, 0, SCL_EFFECT_NONE, 0, NOISE_PERIOD_7 | NOISE_STEP_7BIT
DEF_INSTRUMENT DrumSnare,  %00, $EA, 0, 0, SCL_EFFECT_NONE, 0, NOISE_PERIOD_4 | NOISE_STEP_15BIT
DEF_INSTRUMENT DrumHat,    %00, $FA, 0, 0, SCL_EFFECT_NONE, 0, NOISE_PERIOD_1 | NOISE_STEP_7BIT
DEF_INSTRUMENT DrumCymbal, %00, $F5, 0, 0, SCL_EFFECT_NONE, 0, NOISE_PERIOD_0 | NOISE_STEP_7BIT
DEF_INSTRUMENT WaveOrgan, %00, 0, 0, 0, SCL_EFFECT_NONE, 0, 0, WaveWarm
DEF_INSTRUMENT WaveLead,  %00, 0, 0, 0, SCL_EFFECT_NONE, 0, 0, WaveBright
DEF_INSTRUMENT WaveBuzz,  %00, 0, 0, 0, SCL_EFFECT_NONE, 0, 0, WaveBuzzy

POPS

; ============================================================

endc ; SCL_INC
