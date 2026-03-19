☣️ Ransomware Study Case: AES-256 & WinAPI (C++)
📖 Descrição Técnica
Este projeto é um estudo de caso focado na análise de Criptografia Simétrica em Baixo Nível e técnicas de Engenharia Social/Entrega utilizando a biblioteca Advapi32.lib do Windows. O código demonstra o fluxo técnico completo de um incidente: desde a entrega via Phishing até a cifragem irreversível e auto-deleção.

[!WARNING]

ESTE CÓDIGO É PARA FINS EDUCACIONAIS. A execução em sistemas não autorizados é crime. Utilize apenas em ambientes de sandbox isolados.

🧬 Vetores de Entrega e Evasão
O laboratório aborda diferentes métodos de infecção para testar a resiliência de perímetros de segurança:

📧 Anexo de E-mail (Mascaramento): Arquivo fatura.exe utilizando ícone falso de PDF para induzir a execução via Engenharia Social.

💾 USB Autoplay: Uso de autorun.inf apontando para o binário renomeado como .scr (Screen Saver), explorando a confiança do usuário em mídias removíveis.

💉 DLL Reflexiva (Injeção): Técnica de injeção direta via Macro Excel 4.0 utilizando CreateThread para execução em memória, minimizando rastros em disco.

🛡️ Bypass UAC (User Account Control): Elevação de privilégio silenciosa em apenas 3 linhas de código:

C
if (!IsUserAnAdmin()) { ShellExecute(0, L"runas", self, 0, 0, SW_HIDE); ExitProcess(0); }
🔒 Arquitetura de Criptografia e "Anti-Forensics"
O diferencial deste estudo é a análise da persistência e do descarte de evidências:

Descarte da Chave AES: A chave é gerada e mantida estritamente na RAM, sendo descartada imediatamente após a cifragem. Sem um servidor C&C (Command & Control), a descriptografia torna-se impossível sem a chave original.

Rastro de Rede Zero: Como não há comunicação externa, firewalls de rede e ferramentas de monitoramento de tráfego (IDS/IPS) não detectam anomalias.

Forensics Stealth: O malware não gera arquivos temporários. Peritos digitais encontrarão apenas o executável original e os arquivos já cifrados com a extensão .locked.

🛠️ Compilação de Elite (Foco em Tamanho e Otimização)
Execute os comandos abaixo no Developer Command Prompt for VS para gerar um binário otimizado e sem símbolos de debug:

Bash
# Compilação otimizada para tamanho mínimo e entrypoint customizado
cl /O1 /GS- /link /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup /OUT:enc.exe ransom.c

# Remoção de símbolos para dificultar engenharia reversa básica
strip enc.exe
🧪 Guia de Teste Seguro (VM OFF-LINE)
Prepare uma Máquina Virtual sem conexão com a internet.

Copie o enc.exe para uma pasta de teste com arquivos descartáveis.

Execute: Note que todos os arquivos receberão a extensão .locked.

Análise: Abra o arquivo README_TO_RESTORE.txt para visualizar as instruções de resgate via BTC.

Reboot: Reinicie a VM e observe que o binário original terá desaparecido via auto-deleção programada.

⚖️ Conformidade, Ética e LGPD
Finalidade: Este projeto visa fortalecer defesas de Blue Team e treinar analistas de resposta a incidentes.

LGPD: Este lab desencoraja a coleta de dados reais. O foco é a integridade e disponibilidade dos sistemas, pilares da segurança da informação conforme a Lei 13.709/2018.

Responsabilidade: A Deep Audit Research reforça que o conhecimento ofensivo deve ser usado para proteger a sociedade digital.

Mantido por: [Deep Audit Research]
Aprenda a invadir para saber defender.
