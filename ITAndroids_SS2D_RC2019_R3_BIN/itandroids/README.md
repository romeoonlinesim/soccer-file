Cuidado ao adicionar arquivos ao repositório.
=============================================

NÃO USE GIT ADD ., GIT COMMIT -A, etc.
--------------------------------------

Dê um git add em cada arquivo (se tiver vários use git add -i). Isso vai evitar que o repositório fique cheio de lixo. 
Lembre que não tem como apagar o lixo do repositório, ele sempre vai ficar no histórico, então enviar binários por 
exemplo é estritamente proibido.

NÃO FAÇA COMMITS NA MASTER
============================================

No Soccer2D, a versão anual do time no ano XXXX é guardada na branch ITAndroidsXXXX. Essas versões são mantidas mas 
não atualizadas, a não ser por hotfixes específicos.

A branch principal de desenvolvimento é a **master**, onde ocorrem as integrações entre feature branches que desenvolvem 
features específicas. Dessa maneira, só são permitidos merges na master por pull requests.

Não necessariamente a versão mais recente do time é a mais estável ou a que possui melhor desempenho, então guardamos 
a **versão competitiva** do time  na branch **competitive**. Também só é possível modificar a versão competitiva do time 
com pull requests e de preferência somente após exaustivamente testados.

Outras branches especiais do repositório são:

* READ_ONLY: 
    
    Branch antiga que possui muitos comentários. Pode ser usada para testes e estudo mas deve ser deletada em breve.

* BixaralCompetition

    Branch que guarda a versão do time para a competição de computação dos bixos. Não possui muitas classes de 
    behaviours, actions e decision_making.
    
Quaisquer outras branches são auxiliares e devem ser nomeadas "prefixo"-"nome da branch". O prefixo é utilizado pra
identificar a função da branch e pode ser "fix", "refactor", "feature", "document" ou "other".
Alguns exemplos de uso:

* fix-portability-to-arch: Branch pra corrigir algum problema de portabilidade pra Arch Linux.
* refactor-marking-graph: Branch para reestruturar classes ligadas ao grafo de marcação.
* feature-learning-markees: Branch para implementar a decisão de seleção de markees por rede neural
* document-world-model-lines: Branch para documentar métodos a respeito de lines na classe WorldModel
* other-cute-cat-pics: Branch com algum propósito carteado que não se encaixe nos anteriores. Evite esse prefixo se possível. 


Clonando o repositório e instalando dependências/RoboCup tools
-------------------------------------------

Use o comando `git clone "URL"` e substitua "URL" pelo endereço https de clonagem
do repositório(sem aspas).

É importante saber utilizar GIT pela CLI, mas recomendo o download do software 
GitKraken para facilitar o dia-a-dia. **Atenção** é muito mais fácil fazer besteira 
utilizando o GitKraken então não saia apertando qualquer botão sem saber o que está fazendo.

Para instalar as depedências do Soccer2D, existem 3 scripts de instalação na pasta
/scripts/install/ para Ubuntu12.02, Ubuntu14.04 e Ubuntu16.04. Você pode tentar utilizá-los
em outras distribuições GNU/Linux mas não é garantido funcionar perfeitamente. Futuramente esperamos poder automatizar 
tudo isso utilizando apenas CMake.

Para os aventureiros, você pode usar a *Wiki* para instalar por si só tudo que é necessário 
mas **prefira utilizar os scripts automáticos**. As versões mais recentes das ferramentas RoboCup(datadas de Fev. 2018)
podem ser encontradas na seção *Downloads* desse repositório.

Para inicializar o time, execute o script `start.sh` em /scripts/start/.
Para inicializar o time em modo de debug, execute o script `start-debug.sh`.

Bem vindo à melhor categoria da Androids! #xupa3D