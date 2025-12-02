function quiz_setup(){
    let categories = document.querySelector(".invisiblecategories")
    let button = document.getElementById("option_button")
    let div = document.querySelector(".optionlist")
    let count = 0
    button.addEventListener("click",()=>{
        let deletechild = document.createElement("button")
        deletechild.addEventListener("click",()=>{
            div.removeChild(deletechild.parentElement)
        })
        deletechild.innerHTML="delete"
        let elem = document.createElement("div")
        let nameinput = document.createElement("input")
        let catecopy = categories.cloneNode(true)
        let index_input = document.createElement("input")
        index_input.name = "index"
        index_input.value = count.toString()
        index_input.style["display"] = "none"

        nameinput.name="option_name-"+count.toString()
        catecopy.style["display"] = "block"
                for(let  i = 0; i <catecopy.querySelectorAll("input").length;i++) {
            

catecopy.querySelectorAll("input")[i].name= "category-"+count.toString()

        }
        elem.appendChild(index_input)
        elem.appendChild(nameinput)
        elem.appendChild(deletechild)
        elem.appendChild(catecopy)
        div.appendChild(elem)
        count++;
         
    })
}
quiz_setup()