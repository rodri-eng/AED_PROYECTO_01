import json
import os
import textwrap
from manim import *


class AssociativeQueueScene(Scene):
    def construct(self):
        title = Text(
            "Associative Queue (SWAG)",
            font_size=40,
            color=BLUE
        ).to_edge(UP)

        subtitle = Text(
            "Algoritmos y Estructuras de Datos - UTEC",
            font_size=24,
            color=GRAY
        ).next_to(title, DOWN)

        authors = Text(
            "Integrantes: \nRamos Vargas Royer Sebastian \nHuertos Ochoa Rodrigo Franco \nDiego Antonio Rosario Palomino",
            font_size=20,
            color=WHITE
        ).next_to(subtitle, DOWN, buff=0.5)

        self.play(FadeIn(title), FadeIn(subtitle), FadeIn(authors))
        self.wait(1.5)

        self.play(
            FadeOut(subtitle),
            FadeOut(authors),
            title.animate.scale(0.7).to_corner(UL, buff=0.4)
        )

        # Introducción conceptual
        intro = VGroup(
            Text("Una Associative Queue es una cola FIFO.", font_size=28),
            Text("push agrega al final; pop retira del frente.", font_size=25),
            Text("Acc = mínimo parcial guardado en cada pila.", font_size=25),
            Text("query obtiene el agregado de todos sus elementos.", font_size=25),
            Text("min(min(a, b), c) = min(a, min(b, c))", font_size=24, color=YELLOW),
            Text("Ejemplo: hallar el mínimo de una ventana de datos.", font_size=25),
        ).arrange(DOWN, buff=0.35).move_to(ORIGIN)

        self.play(FadeIn(intro))
        self.wait(7)
        self.play(FadeOut(intro))

        json_path = os.path.join(os.path.dirname(__file__), "trace.json")

        if not os.path.exists(json_path):
            error_txt = Text(
                "Error: No se encontró trace.json",
                font_size=24,
                color=RED
            )
            self.add(error_txt)
            return

        with open(json_path, "r", encoding="utf-8") as f:
            trace_steps = json.load(f)

        box_in = Rectangle(
            width=3.0,
            height=3.8,
            color=BLUE_D
        ).shift(LEFT * 3.6 + DOWN * 1.0)

        label_in = Text(
            "Stack IN (Push)",
            font_size=20,
            color=BLUE
        ).next_to(box_in, UP, buff=0.2)

        box_out = Rectangle(
            width=3.0,
            height=3.8,
            color=TEAL_D
        ).shift(RIGHT * 3.6 + DOWN * 1.0)

        label_out = Text(
            "Stack OUT (Pop)",
            font_size=20,
            color=TEAL
        ).next_to(box_out, UP, buff=0.2)

        agg_box = RoundedRectangle(
            corner_radius=0.15,
            width=4.2,
            height=1.0,
            color=YELLOW
        ).shift(UP * 2.4)

        agg_title = Text(
            "Agregado Global (min):",
            font_size=20,
            color=YELLOW_B
        ).next_to(agg_box, UP, buff=0.15)

        agg_val_text = Text(
            "--",
            font_size=28,
            color=YELLOW
        ).move_to(agg_box.get_center())

        desc_text = Text(
            "Iniciando demostración...",
            font_size=18,
            color=LIGHT_GRAY
        ).to_edge(DOWN, buff=0.4)

        self.play(
            Create(box_in),
            Write(label_in),
            Create(box_out),
            Write(label_out),
            Create(agg_box),
            Write(agg_title),
            Write(agg_val_text),
            Write(desc_text)
        )

        self.wait(1)

        current_in_mobjects = []
        current_out_mobjects = []

        def make_element(val, acc, base_color):
            card = RoundedRectangle(
                corner_radius=0.1,
                width=2.6,
                height=0.6,
                color=base_color,
                fill_opacity=0.25
            )

            txt = Text(
                f"Val: {val} | Acc: {acc}",
                font_size=16,
                color=WHITE
            )
            txt.move_to(card.get_center())

            return VGroup(card, txt)

        for step in trace_steps:
            op = step["op"]
            desc = step["desc"]
            total_agg = step["total_agg"]

            new_desc = Text(
                textwrap.fill(desc, width=55, break_long_words=False),
                font_size=18,
                color=LIGHT_GRAY
            ).to_edge(DOWN, buff=0.25)

            new_agg_str = str(total_agg) if total_agg is not None else "--"

            new_agg_val = Text(
                new_agg_str,
                font_size=28,
                color=YELLOW
            ).move_to(agg_box.get_center())

            anim_list = [
                Transform(desc_text, new_desc),
                Transform(agg_val_text, new_agg_val),
            ]

            if op == "ERROR_POP":
                flash_box = SurroundingRectangle(box_out, color=RED, buff=0.1)
                self.play(*anim_list, Create(flash_box), run_time=0.6)
                self.play(FadeOut(flash_box), run_time=0.4)

            elif op == "PUSH":
                in_data = step["stack_in"]
                last_elem = in_data[-1]

                elem_mob = make_element(
                    last_elem["val"],
                    last_elem["acc"],
                    BLUE
                )

                idx = len(in_data) - 1
                target_pos = box_in.get_bottom() + UP * (0.45 + idx * 0.7)

                # La tarjeta aparece dentro de la pila, debajo del rótulo.
                elem_mob.move_to(box_in.get_top() + DOWN * 0.45)

                self.play(*anim_list, FadeIn(elem_mob), run_time=0.4)
                self.play(elem_mob.animate.move_to(target_pos), run_time=0.6)

                current_in_mobjects.append(elem_mob)

            elif op == "START_TRANSFER":
                self.play(*anim_list, run_time=0.8)

            elif op == "TRANSFER_ITEM":
                # El último objeto de IN representa el tope que acaba de salir.
                moving_mob = current_in_mobjects.pop()

                out_data = step["stack_out"]
                transferred = out_data[-1]
                idx = len(out_data) - 1

                target_pos = box_out.get_bottom() + UP * (0.45 + idx * 0.7)

                target_mob = make_element(
                    transferred["val"],
                    transferred["acc"],
                    TEAL
                ).move_to(target_pos)

                self.play(
                    *anim_list,
                    Transform(moving_mob, target_mob),
                    run_time=1.2
                )

                current_out_mobjects.append(moving_mob)

            elif op == "END_TRANSFER":
                self.play(*anim_list, run_time=0.5)

            elif op == "QUERY":
                out_acc = step["stack_out"][-1]["acc"]
                in_acc = step["stack_in"][-1]["acc"]

                formula = Text(
                    f"min({out_acc}, {in_acc}) = {total_agg}",
                    font_size=26,
                    color=YELLOW
                ).move_to(ORIGIN)

                self.play(
                    *anim_list,
                    Indicate(current_out_mobjects[-1]),
                    Indicate(current_in_mobjects[-1]),
                    run_time=1.2
                )
                self.play(FadeIn(formula))
                self.wait(2)
                self.play(FadeOut(formula))

            elif op == "POP":
                if current_out_mobjects:
                    popped_mob = current_out_mobjects.pop()
                    self.play(
                        *anim_list,
                        popped_mob.animate.shift(
                            RIGHT * 1.8 + UP * 0.4
                        ).set_opacity(0),
                        run_time=0.8,
                    )
                    self.remove(popped_mob)
                else:
                    self.play(*anim_list, run_time=0.5)

            self.wait(0.5)

        self.play(
            FadeOut(desc_text),
            FadeOut(box_in),
            FadeOut(box_out),
            FadeOut(label_in),
            FadeOut(label_out),
            FadeOut(agg_box),
            FadeOut(agg_title),
            FadeOut(agg_val_text),
        )

        complexity_title = Text(
            "Análisis de Complejidad Temporal",
            font_size=28,
            color=GREEN
        ).shift(UP * 2)

        c1 = Text(
            "- Push: O(1) en cada operación",
            font_size=22
        ).next_to(complexity_title, DOWN, buff=0.4).align_to(complexity_title, LEFT)

        c2 = Text(
            "- Pop: O(n) en el peor caso; O(1) amortizado",
            font_size=22
        ).next_to(c1, DOWN, buff=0.3).align_to(c1, LEFT)

        c3 = Text(
            "- Query / Fold: O(1) estricto en el peor caso",
            font_size=22
        ).next_to(c2, DOWN, buff=0.3).align_to(c1, LEFT)

        self.play(
            Write(complexity_title),
            FadeIn(c1),
            FadeIn(c2),
            FadeIn(c3),
        )

        self.wait(2.5)

        self.play(
            FadeOut(complexity_title),
            FadeOut(c1),
            FadeOut(c2),
            FadeOut(c3),
            FadeOut(title),
        )

        self.wait(0.5)