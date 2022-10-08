// РАДИО МОДУЛЬ

import {h, Component} from "preact";
import Message from "./Message";
import Button from "./Button";
import Input from "./Input";

export default class Service extends Component {
  constructor() {
    super();
    this.setState({
      expanded: {
        service: false
      }
    });
  }


  render({}) {

    return (

      <div className="sb">
        {/* СЕРВИС */}
        <input type="checkbox" id="service" className="checker"
               checked={this.state.expanded.service}
               onChange={this.linkState("expanded.service")}/>
        <label for="service"><p className="tsb"><Message key="s.m.service.t"/></p></label>
        <div className="spl">
          {/* Статуса датчиков */}
          <p className="bold"><Message key="s.m.service.statusSensors.t"/></p>
          <br/>
          {/* Полный сброс настроек */}
          <p className="bold"><Message key="s.m.sys.def"/></p>
          <Button className="button w75 red large" onClick={() => this.context.sendWS("def")}><Message
            key="a.perform"/></Button>
        </div>
      </div>
    );
  }
}