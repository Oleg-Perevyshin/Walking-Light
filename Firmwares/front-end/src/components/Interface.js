// РАДИО МОДУЛЬ

import {h, Component} from "preact";
import Message from "./Message";
import Button from "./Button";
import Input from "./Input";

export default class Interface extends Component {
  constructor() {
    super();
    this.setState({
      expanded: {
        interface: false
      },
      themes: [
        "s.m.interface.themes.light",
        "s.m.interface.themes.dark"
      ]
    });
  }


  render({}, {themes}) {
    const {dls, setLang, setTheme} = this.context;

    return (

      <div className="sb">
        {/* ИНТЕРФЕЙС */}
        <input type="checkbox" id="interface" className="checker"
               checked={this.state.expanded.interface}
               onChange={this.linkState("expanded.interface")}/>
        <label for="interface"><p className="tsb"><Message key="s.m.interface.t"/></p>
        </label>
        <div className="spl">
          {/* Язык интефейса */}
          <p className="bold"><Message key="s.m.interface.language"/></p>
          {dls.map(dl =>
            <Button
              className={"flex-elem button blue large" + (dl === this.context.dl ? " active" : "")}
              onClick={setLang}
              value={dl}>
              {dl.toUpperCase()}
            </Button>
          )}
          {/* Тема оформления */}
          <p className="bold"><Message key="s.m.interface.dv"/></p>
          {themes.map(dv =>
            <Button className={"button w30 blue large" +
            (themes.indexOf(dv) === parseInt(this.context.dv) ? " active" : "")
            }
                    onClick={setTheme}
                    value={themes.indexOf(dv)}>
              <Message key={dv}/>
            </Button>
          )}
          <p/>
        </div>
      </div>
    );
  }
}